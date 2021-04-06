/*
 * Copyright (c) 2020 The Jaeger Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JAEGERTRACING_PROPAGATION_W3CPROPAGATOR_H
#define JAEGERTRACING_PROPAGATION_W3CPROPAGATOR_H

#include "jaegertracing/propagation/Propagator.h"
#include "jaegertracing/utils/HexParsing.h"

namespace jaegertracing {
namespace propagation {

template <typename ReaderType, typename WriterType>
class W3CPropagator : public Propagator<ReaderType, WriterType> {
  public:
    using Reader = ReaderType;
    using Writer = WriterType;
    using StrMap = SpanContext::StrMap;

    using Propagator<Reader, Writer>::Propagator;

  protected:
    SpanContext doExtract(const Reader& reader) const override
    {
        SpanContext ctx;
        std::string traceState;
        const auto result = reader.ForeachKey(
            [this, &ctx, &traceState](const std::string& rawKey,
                                      const std::string& value) {
                const auto key = this->normalizeKey(rawKey);
                if (key == kW3CTraceParentHeaderName) {
                    const auto safeValue = this->decodeValue(value);
                    std::istringstream iss(safeValue);
                    ctx = readTraceParent(iss);
                    if (!iss || ctx == SpanContext()) {
                        return opentracing::make_expected_from_error<void>(
                            opentracing::span_context_corrupted_error);
                    }
                }
                else if (key == kW3CTraceStateHeaderName) {
                    traceState = this->decodeValue(value);
                }

                return opentracing::make_expected();
            });

        if (!result &&
            result.error() == opentracing::span_context_corrupted_error) {
            this->_metrics->decodingErrors().inc(1);
            return SpanContext();
        }

        return SpanContext(ctx.traceID(),
                           ctx.spanID(),
                           ctx.parentID(),
                           ctx.flags(),
                           StrMap(),
                           "",
                           traceState);
    }

    void doInject(const SpanContext& ctx, const Writer& writer) const override
    {
        std::ostringstream oss;
        writeTraceParent(oss, ctx);
        writer.Set(kW3CTraceParentHeaderName, oss.str());
        if (!ctx.traceState().empty()) {
            writer.Set(kW3CTraceStateHeaderName, ctx.traceState());
        }
    }

  private:
    SpanContext readTraceParent(std::istream& in) const
    {
        char ch = '\0';
        if (!in.get(ch) || ch != '0') {
            return SpanContext();
        }

        if (!in.get(ch) || ch != '0') {
            return SpanContext();
        }

        if (!in.get(ch) || ch != '-') {
            return SpanContext();
        }

        auto traceID = readTraceID(in);
        if (!traceID.isValid()) {
            return SpanContext();
        }

        if (!in.get(ch) || ch != '-') {
            return SpanContext();
        }

        constexpr auto kMaxUInt64Chars = static_cast<size_t>(16);
        auto buffer = utils::HexParsing::readSegment(in, kMaxUInt64Chars, '-');
        if (buffer.empty()) {
            return SpanContext();
        }
        auto spanID = utils::HexParsing::decodeHex<uint64_t>(buffer);

        if (!in.get(ch) || ch != '-') {
            return SpanContext();
        }

        constexpr auto kMaxByteChars = static_cast<size_t>(2);
        buffer = utils::HexParsing::readSegment(in, kMaxByteChars, '-');
        if (buffer.empty() || buffer.size() != 2) {
            return SpanContext();
        }
        auto flags = utils::HexParsing::decodeHex<unsigned char>(buffer);

        in.clear();

        return SpanContext(traceID, spanID, 0, flags, StrMap());
    }

    TraceID readTraceID(std::istream& in) const
    {
        TraceID traceID;
        constexpr auto kMaxChars = static_cast<size_t>(32);
        auto buffer = utils::HexParsing::readSegment(in, kMaxChars, '-');

        if (buffer.empty() || buffer.size() != kMaxChars) {
            return TraceID();
        }

        auto beginLowStr = std::end(buffer) - kMaxChars / 2;
        const std::string highStr(std::begin(buffer), beginLowStr);
        auto high = utils::HexParsing::decodeHex<uint64_t>(highStr);
        const std::string lowStr(beginLowStr, std::end(buffer));
        auto low = utils::HexParsing::decodeHex<uint64_t>(lowStr);

        return TraceID(high, low);
    }

    void writeTraceParent(std::ostream& out, const SpanContext& ctx) const
    {
        out << "00";
        out << '-';
        writeTraceID(out, ctx.traceID());
        out << '-';
        out << std::setw(16) << std::setfill('0') << std::hex << ctx.spanID();
        out << '-';
        out << std::setw(2) << std::setfill('0') << std::hex
            << static_cast<size_t>(ctx.flags());
    }

    void writeTraceID(std::ostream& out, const TraceID& traceID) const
    {
        out << std::setw(16) << std::setfill('0') << std::hex << traceID.high();
        out << std::setw(16) << std::setfill('0') << std::hex << traceID.low();
    }
};

using W3CTextMapPropagator = W3CPropagator<const opentracing::TextMapReader&,
                                           const opentracing::TextMapWriter&>;

class W3CHTTPHeaderPropagator
    : public W3CPropagator<const opentracing::HTTPHeadersReader&,
                           const opentracing::HTTPHeadersWriter&> {
  public:
    using W3CPropagator<Reader, Writer>::W3CPropagator;

  protected:
    std::string encodeValue(const std::string& str) const override
    {
        return net::URI::queryEscape(str);
    }

    std::string decodeValue(const std::string& str) const override
    {
        return net::URI::queryUnescape(str);
    }

    std::string normalizeKey(const std::string& rawKey) const override
    {
        std::string key;
        key.reserve(rawKey.size());
        std::transform(std::begin(rawKey),
                       std::end(rawKey),
                       std::back_inserter(key),
                       [](char ch) { return std::tolower(ch); });
        return key;
    }
};

}  // namespace propagation
}  // namespace jaegertracing

#endif  // JAEGERTRACING_PROPAGATION_W3CPROPAGATOR_H
