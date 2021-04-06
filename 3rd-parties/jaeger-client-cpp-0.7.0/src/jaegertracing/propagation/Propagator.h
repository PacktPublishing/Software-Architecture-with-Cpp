/*
 * Copyright (c) 2017 Uber Technologies, Inc.
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

#ifndef JAEGERTRACING_PROPAGATION_PROPAGATOR_H
#define JAEGERTRACING_PROPAGATION_PROPAGATOR_H

#include "jaegertracing/SpanContext.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/net/URI.h"
#include "jaegertracing/platform/Endian.h"
#include "jaegertracing/propagation/Extractor.h"
#include "jaegertracing/propagation/HeadersConfig.h"
#include "jaegertracing/propagation/Injector.h"
#include <cctype>
#include <climits>
#include <opentracing/propagation.h>
#include <sstream>

namespace jaegertracing {

class Tracer;

namespace propagation {

template <typename ReaderType, typename WriterType>
class Propagator : public Extractor<ReaderType>, public Injector<WriterType> {
  public:
    using Reader = ReaderType;
    using Writer = WriterType;

    Propagator()
        : _headerKeys()
        , _metrics(metrics::Metrics::makeNullMetrics())
    {
    }

    Propagator(const HeadersConfig& headerKeys,
               const std::shared_ptr<metrics::Metrics>& metrics)
        : _headerKeys(headerKeys)
        , _metrics(metrics)
    {
    }

    virtual ~Propagator() = default;

    SpanContext extract(const Reader& reader) const override
    {
        std::string debugID;
        const auto result =
            reader.ForeachKey([this, &debugID](const std::string& rawKey,
                                               const std::string& value) {
                const auto key = normalizeKey(rawKey);
                if (key == _headerKeys.jaegerDebugHeader()) {
                    debugID = value;
                }

                return opentracing::make_expected();
            });

        SpanContext ctx = doExtract(reader);
        if (!ctx.traceID().isValid() && ctx.baggage().empty() &&
            debugID.empty()) {
            return SpanContext();
        }

        int flags = ctx.flags();
        if (!debugID.empty()) {
            flags |= static_cast<unsigned char>(SpanContext::Flag::kDebug) |
                     static_cast<unsigned char>(SpanContext::Flag::kSampled);
        }
        return SpanContext(ctx.traceID(),
                           ctx.spanID(),
                           ctx.parentID(),
                           flags,
                           ctx.baggage(),
                           debugID,
                           ctx.traceState());
    }

    void inject(const SpanContext& ctx, const Writer& writer) const override
    {
        doInject(ctx, writer);
    }

  protected:
    virtual SpanContext doExtract(const Reader& reader) const = 0;

    virtual void doInject(const SpanContext& ctx, const Writer& writer) const = 0;

    virtual std::string encodeValue(const std::string& str) const
    {
        return str;
    }

    virtual std::string decodeValue(const std::string& str) const
    {
        return str;
    }

    virtual std::string normalizeKey(const std::string& rawKey) const
    {
        return rawKey;
    }

    HeadersConfig _headerKeys;
    std::shared_ptr<metrics::Metrics> _metrics;
};

class BinaryPropagator : public Extractor<std::istream&>,
                         public Injector<std::ostream&> {
  public:
    using StrMap = SpanContext::StrMap;

    explicit BinaryPropagator(const std::shared_ptr<metrics::Metrics>& metrics =
                                  std::shared_ptr<metrics::Metrics>())
        : _metrics(metrics == nullptr ? metrics::Metrics::makeNullMetrics()
                                      : metrics)
    {
    }

    void inject(const SpanContext& ctx, std::ostream& out) const override
    {
        writeBinary(out, ctx.traceID().high());
        writeBinary(out, ctx.traceID().low());
        writeBinary(out, ctx.spanID());
        writeBinary(out, ctx.parentID());
        // `flags` is a single byte, so endianness is not an issue.
        out.put(ctx.flags());

        writeBinary(out, static_cast<uint32_t>(ctx.baggage().size()));
        for (auto&& pair : ctx.baggage()) {
            auto&& key = pair.first;
            writeBinary(out, static_cast<uint32_t>(key.size()));
            out.write(key.c_str(), key.size());

            auto&& value = pair.second;
            writeBinary(out, static_cast<uint32_t>(value.size()));
            out.write(value.c_str(), value.size());
        }
    }

    SpanContext extract(std::istream& in) const override
    {
        const auto traceIDHigh = readBinary<uint64_t>(in);
        const auto traceIDLow = readBinary<uint64_t>(in);
        TraceID traceID(traceIDHigh, traceIDLow);
        const auto spanID = readBinary<uint64_t>(in);
        const auto parentID = readBinary<uint64_t>(in);

        auto ch = '\0';
        in.get(ch);
        const auto flags = static_cast<unsigned char>(ch);

        const auto numBaggageItems = readBinary<uint32_t>(in);
        StrMap baggage;
        baggage.reserve(numBaggageItems);
        for (auto i = static_cast<uint32_t>(0); i < numBaggageItems; ++i) {
            const auto keyLength = readBinary<uint32_t>(in);
            std::string key(keyLength, '\0');
            if (!in.read(&key[0], keyLength)) {
                _metrics->decodingErrors().inc(1);
                return SpanContext();
            }

            const auto valueLength = readBinary<uint32_t>(in);
            std::string value(valueLength, '\0');
            if (!in.read(&value[0], valueLength)) {
                _metrics->decodingErrors().inc(1);
                return SpanContext();
            }

            baggage[key] = value;
        }

        SpanContext ctx(traceID, spanID, parentID, flags, baggage);
        return ctx;
    }

  private:
    template <typename ValueType>
    static
        typename std::enable_if<std::is_integral<ValueType>::value, void>::type
        writeBinary(std::ostream& out, ValueType value)
    {
        const ValueType outValue = platform::endian::toBigEndian(value);
        for (auto i = static_cast<size_t>(0); i < sizeof(ValueType); ++i) {
            const auto numShiftBits = (sizeof(ValueType) - i - 1) * CHAR_BIT;
            const auto byte = outValue >> numShiftBits;
            out.put(static_cast<unsigned char>(byte));
        }
    }

    template <typename ValueType>
    static typename std::enable_if<std::is_integral<ValueType>::value,
                                   ValueType>::type
    readBinary(std::istream& in)
    {
        auto value = static_cast<ValueType>(0);
        auto ch = '\0';
        for (auto i = static_cast<size_t>(0);
             i < sizeof(ValueType) && in.get(ch);
             ++i) {
            const auto byte = static_cast<uint8_t>(ch);
            value <<= CHAR_BIT;
            value |= byte;
        }
        return platform::endian::fromBigEndian(value);
    }

  private:
    std::shared_ptr<metrics::Metrics> _metrics;
};

}  // namespace propagation
}  // namespace jaegertracing

#endif  // JAEGERTRACING_PROPAGATION_PROPAGATOR_H
