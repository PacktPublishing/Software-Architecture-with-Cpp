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

#include "jaegertracing/propagation/W3CPropagator.h"
#include <gtest/gtest.h>

namespace jaegertracing {
namespace propagation {

using StrMap = std::unordered_map<std::string, std::string>;

template <typename BaseWriter>
struct WriterMock : public BaseWriter {
    explicit WriterMock(StrMap& keyValuePairs)
        : _keyValuePairs(keyValuePairs)
    {
    }

    opentracing::expected<void>
    Set(opentracing::string_view key,
        opentracing::string_view value) const override
    {
        _keyValuePairs[key] = value;
        return opentracing::make_expected();
    }

    StrMap& _keyValuePairs;
};

template <typename BaseReader>
struct ReaderMock : public BaseReader {
    using Function =
        std::function<bool(opentracing::string_view, opentracing::string_view)>;

    explicit ReaderMock(const StrMap& keyValuePairs)
        : _keyValuePairs(keyValuePairs)
    {
    }

    opentracing::expected<void> ForeachKey(
        std::function<opentracing::expected<void>(opentracing::string_view,
                                                  opentracing::string_view)> f)
        const override
    {
        for (auto&& pair : _keyValuePairs) {
            const auto result = f(pair.first, pair.second);
            if (!result) {
                return result;
            }
        }
        return opentracing::make_expected();
    }

    const StrMap& _keyValuePairs;
};

struct ExtractTestCase {
    std::string _input;
    bool _success;
};

TEST(W3CPropagator, testExtract)
{
    const ExtractTestCase testCases[] = {
        { "00-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", true },
        { "00-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-00", true },
        { "00-11111111111111111111111111111111-2222222222222222-01", true },
        { "00-AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-BBBBBBBBBBBBBBBB-01", true },
        { "", false },
        { "000af7651916cd43dd8448eb211c80319cb9c7c989f97918e101", false },
        { "000af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "00-0af7651916cd43dd8448eb211c80319cb9c7c989f97918e1-01", false },
        { "00-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e101", false },
        { "000af7651916cd43dd8448eb211c80319cb9c7c989f97918e1-01", false },
        { "00-0af7651916cd43dd8448eb211c80319cb9c7c989f97918e101", false },
        { "01-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "0-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "000-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "0k-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "k0-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01", false },
        { "00-00000000000000000000000000000000-b9c7c989f97918e1-01", false },
        { "01-0af7651916cd43dd8448eb211c80319-b9c7c989f97918e1-01", false },
        { "01-0af7651916cd43dd8448eb211c80319cc-b9c7c989f97918e1-01", false },
        { "00-0af7651916cd43dd8448eb211c80319c-0000000000000000-01", false }
    };

    W3CTextMapPropagator textMapPropagator;
    W3CHTTPHeaderPropagator httpHeaderPropagator;
    for (auto&& testCase : testCases) {
        SpanContext spanContext;
        {
            StrMap map;
            map.emplace(kW3CTraceParentHeaderName, testCase._input);

            spanContext = textMapPropagator.extract(
                ReaderMock<opentracing::TextMapReader>(map));
            ASSERT_EQ(testCase._success, spanContext.isValid())
                << "input=" << testCase._input;

            spanContext = httpHeaderPropagator.extract(
                ReaderMock<opentracing::HTTPHeadersReader>(map));
            ASSERT_EQ(testCase._success, spanContext.isValid())
                << "input=" << testCase._input;
        }
    }

    StrMap map;
    map.emplace(kW3CTraceParentHeaderName,
                "00-00000000000000ff0000000000000001-0000000000000002-01");
    map.emplace(kW3CTraceStateHeaderName, "foo=bar");

    SpanContext spanContext =
        textMapPropagator.extract(ReaderMock<opentracing::TextMapReader>(map));
    ASSERT_EQ(true, spanContext.isValid());
    ASSERT_EQ("foo=bar", spanContext.traceState());

    spanContext = httpHeaderPropagator.extract(
        ReaderMock<opentracing::HTTPHeadersReader>(map));
    ASSERT_EQ(true, spanContext.isValid());
    ASSERT_EQ("foo=bar", spanContext.traceState());
}

TEST(W3CPropagator, testInject)
{
    SpanContext spanContext(
        TraceID(255, 1), 2, 3, 1, SpanContext::StrMap(), "", "foo=bar");

    StrMap textMap;
    WriterMock<opentracing::TextMapWriter> textMapWriter(textMap);

    W3CTextMapPropagator textMapPropagator;
    textMapPropagator.inject(spanContext, textMapWriter);

    ASSERT_EQ("00-00000000000000ff0000000000000001-0000000000000002-01",
              textMap.at(kW3CTraceParentHeaderName));
    ASSERT_EQ("foo=bar", textMap.at(kW3CTraceStateHeaderName));

    StrMap httpHeaderMap;
    WriterMock<opentracing::HTTPHeadersWriter> httpHeaderWriter(httpHeaderMap);

    W3CHTTPHeaderPropagator httpHeaderPropagator;
    httpHeaderPropagator.inject(spanContext, httpHeaderWriter);

    ASSERT_EQ("00-00000000000000ff0000000000000001-0000000000000002-01",
              httpHeaderMap.at(kW3CTraceParentHeaderName));
    ASSERT_EQ("foo=bar", textMap.at(kW3CTraceStateHeaderName));
}

}  // namespace propagation
}  // namespace jaegertracing
