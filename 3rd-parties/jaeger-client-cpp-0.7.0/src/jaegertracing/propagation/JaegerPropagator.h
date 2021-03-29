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

#ifndef JAEGERTRACING_PROPAGATION_JAEGERPROPAGATOR_H
#define JAEGERTRACING_PROPAGATION_JAEGERPROPAGATOR_H

#include "jaegertracing/propagation/Propagator.h"

namespace jaegertracing {
namespace propagation {

template <typename ReaderType, typename WriterType>
class JaegerPropagator : public Propagator<ReaderType, WriterType> {
  public:
    using Reader = ReaderType;
    using Writer = WriterType;
    using StrMap = SpanContext::StrMap;

    using Propagator<Reader, Writer>::Propagator;

  protected:
    SpanContext doExtract(const Reader& reader) const override
    {
        SpanContext ctx;
        StrMap baggage;
        const auto result =
            reader.ForeachKey([this, &ctx, &baggage](const std::string& rawKey,
                                                     const std::string& value) {
                const auto key = this->normalizeKey(rawKey);
                if (key == this->_headerKeys.traceContextHeaderName()) {
                    const auto safeValue = this->decodeValue(value);
                    std::istringstream iss(safeValue);
                    if (!(iss >> ctx) || ctx == SpanContext()) {
                        return opentracing::make_expected_from_error<void>(
                            opentracing::span_context_corrupted_error);
                    }
                }
                else if (key == this->_headerKeys.jaegerBaggageHeader()) {
                    for (auto&& pair : parseCommaSeparatedMap(value)) {
                        baggage[pair.first] = pair.second;
                    }
                }
                else {
                    const auto prefix =
                        this->_headerKeys.traceBaggageHeaderPrefix();
                    if (key.size() >= prefix.size() &&
                        key.substr(0, prefix.size()) == prefix) {
                        const auto safeKey = removeBaggageKeyPrefix(key);
                        const auto safeValue = this->decodeValue(value);
                        baggage[safeKey] = safeValue;
                    }
                }

                return opentracing::make_expected();
            });

        if (!result &&
            result.error() == opentracing::span_context_corrupted_error) {
            this->_metrics->decodingErrors().inc(1);
            return SpanContext();
        }

        return SpanContext(
            ctx.traceID(), ctx.spanID(), ctx.parentID(), ctx.flags(), baggage);
    }

    void doInject(const SpanContext& ctx, const Writer& writer) const override
    {
        std::ostringstream oss;
        oss << ctx;
        writer.Set(this->_headerKeys.traceContextHeaderName(), oss.str());
        ctx.forEachBaggageItem(
            [this, &writer](const std::string& key, const std::string& value) {
                const auto safeKey = addBaggageKeyPrefix(key);
                const auto safeValue = this->encodeValue(value);
                writer.Set(safeKey, safeValue);
                return true;
            });
    }

  private:
    static StrMap parseCommaSeparatedMap(const std::string& escapedValue)
    {
        StrMap map;
        std::istringstream iss(net::URI::queryUnescape(escapedValue));
        std::string piece;
        while (std::getline(iss, piece, ',')) {
            const auto eqPos = piece.find('=');
            if (eqPos != std::string::npos) {
                const auto key = piece.substr(0, eqPos);
                const auto value = piece.substr(eqPos + 1);
                map[key] = value;
            }
        }
        return map;
    }

    std::string addBaggageKeyPrefix(const std::string& key) const
    {
        return this->_headerKeys.traceBaggageHeaderPrefix() + key;
    }

    std::string removeBaggageKeyPrefix(const std::string& key) const
    {
        return key.substr(this->_headerKeys.traceBaggageHeaderPrefix().size());
    }
};

using JaegerTextMapPropagator =
    JaegerPropagator<const opentracing::TextMapReader&,
                     const opentracing::TextMapWriter&>;

class JaegerHTTPHeaderPropagator
    : public JaegerPropagator<const opentracing::HTTPHeadersReader&,
                              const opentracing::HTTPHeadersWriter&> {
  public:
    using JaegerPropagator<Reader, Writer>::JaegerPropagator;

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

#endif  // JAEGERTRACING_PROPAGATION_JAEGERPROPAGATOR_H
