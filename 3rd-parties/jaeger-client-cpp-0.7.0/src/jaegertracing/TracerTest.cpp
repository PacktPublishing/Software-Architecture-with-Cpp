/*
 * Copyright (c) 2017-2018 Uber Technologies, Inc.
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

#include "jaegertracing/Config.h"
#include "jaegertracing/Constants.h"
#include "jaegertracing/Span.h"
#include "jaegertracing/SpanContext.h"
#include "jaegertracing/Tag.h"
#include "jaegertracing/TraceID.h"
#include "jaegertracing/Tracer.h"
#include "jaegertracing/baggage/RestrictionsConfig.h"
#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/propagation/HeadersConfig.h"
#include "jaegertracing/reporters/Config.h"
#include "jaegertracing/samplers/Config.h"
#include "jaegertracing/testutils/TracerUtil.h"
#include <algorithm>
#include <chrono>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <opentracing/expected/expected.hpp>
#include <opentracing/noop.h>
#include <opentracing/propagation.h>
#include <opentracing/span.h>
#include <opentracing/string_view.h>
#include <opentracing/tracer.h>
#include <opentracing/util.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace jaegertracing {

using StrMap = SpanContext::StrMap;

namespace {

class FakeSpanContext : public opentracing::SpanContext {
    void ForeachBaggageItem(
        std::function<bool(const std::string&,
                           const std::string&)> /* unused */) const override
    {
        // Do nothing
    }

    virtual std::unique_ptr<SpanContext> Clone() const noexcept override
    {
        return std::unique_ptr<FakeSpanContext>(new FakeSpanContext());
    }
};

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

template <typename ClockType>
typename ClockType::duration
absTimeDiff(const typename ClockType::time_point& lhs,
            const typename ClockType::time_point& rhs)
{
    return (rhs < lhs) ? (lhs - rhs) : (rhs - lhs);
}

}  // anonymous namespace

TEST(Tracer, testTracer)
{
    {

    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer =
        std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());

    auto tagItr = std::find_if(
        std::begin(tracer->tags()),
        std::end(tracer->tags()),
        [](const Tag& tag) { return tag.key() == kJaegerClientVersionTagKey; });
    ASSERT_NE(std::end(tracer->tags()), tagItr);
    ASSERT_TRUE(tagItr->value().is<const char*>());
    ASSERT_EQ("C++-",
              static_cast<std::string>(tagItr->value().get<const char*>())
                  .substr(0, 4));

    opentracing::StartSpanOptions options;
    options.tags.push_back({ "tag-key", 1.23 });

    const FakeSpanContext fakeCtx;
    options.references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                                    &fakeCtx);
    const SpanContext emptyCtx(TraceID(), 0, 0, 0, StrMap());
    options.references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                                    &emptyCtx);
    const SpanContext parentCtx(
        TraceID(0xDEAD, 0xBEEF), 0xBEEF, 1234, 0, StrMap());
    options.references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                                    &parentCtx);
    options.references.emplace_back(
        opentracing::SpanReferenceType::FollowsFromRef, &parentCtx);
    const SpanContext debugCtx(
        TraceID(),
        0,
        0,
        static_cast<unsigned char>(SpanContext::Flag::kSampled) |
            static_cast<unsigned char>(SpanContext::Flag::kDebug),
        StrMap({ { "debug-baggage-key", "debug-baggage-value" } }),
        "123");
    options.references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                                    &debugCtx);

    const auto& tags = tracer->tags();
    auto itr =
        std::find_if(std::begin(tags), std::end(tags), [](const Tag& tag) {
            return tag.key() == kTracerIPTagKey;
        });
    ASSERT_NE(std::end(tags), itr);
    ASSERT_TRUE(itr->value().is<std::string>());
    ASSERT_EQ(net::IPAddress::v4(itr->value().get<std::string>()).host(),
              net::IPAddress::localIP(AF_INET).host());

    std::unique_ptr<Span> span(static_cast<Span*>(
        tracer->StartSpanWithOptions("test-operation", options).release()));
    ASSERT_TRUE(static_cast<bool>(span));
    ASSERT_EQ(static_cast<opentracing::Tracer*>(tracer.get()), &span->tracer());

    span->SetOperationName("test-set-operation");
    span->SetTag("tag-key", "tag-value");
    span->SetBaggageItem("test-baggage-item-key", "test baggage item value");
    ASSERT_EQ("test baggage item value",
              span->BaggageItem("test-baggage-item-key"));
    span->Log({ { "log-bool", true } });
    opentracing::FinishSpanOptions foptions;
    opentracing::LogRecord lr{};
    lr.fields = { { "options-log", "yep" } };
    foptions.log_records.push_back(std::move(lr));
    lr.timestamp = opentracing::SystemClock::now();
    span->FinishWithOptions(foptions);
    ASSERT_GE(Span::SteadyClock::now(),
              span->startTimeSteady() + span->duration());
    span->SetOperationName("test-set-operation-after-finish");
    ASSERT_EQ("test-set-operation", span->operationName());
    span->SetTag("tagged-after-finish-key", "tagged-after-finish-value");

    span.reset(static_cast<Span*>(
        tracer->StartSpanWithOptions("test-span-with-default-options", {})
            .release()));

    options.references.clear();
    options.references.emplace_back(
        opentracing::SpanReferenceType::FollowsFromRef, &parentCtx);
    span.reset(static_cast<Span*>(
        tracer->StartSpanWithOptions("test-span-with-default-options", options)
            .release()));

    options.references.clear();
    options.references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                                    &debugCtx);
    span.reset(static_cast<Span*>(
        tracer->StartSpanWithOptions("test-span-with-debug-parent", options)
            .release()));

    options.references.clear();
    options.references.emplace_back(
        opentracing::SpanReferenceType::FollowsFromRef, &parentCtx);
    options.start_steady_timestamp = Tracer::SteadyClock::now();
    span.reset(static_cast<Span*>(
        tracer
            ->StartSpanWithOptions("test-span-with-default-system-timestamp",
                                   options)
            .release()));
    /*
    TODO: https://github.com/jaegertracing/jaeger-client-cpp/issues/167
    const auto calculatedSystemTime =
        static_cast<Tracer::SystemClock::time_point>(
            opentracing::convert_time_point<Tracer::SystemClock>(
                span->startTimeSteady()));
    ASSERT_GE(std::chrono::milliseconds(10),
              absTimeDiff<Tracer::SystemClock>(span->startTimeSystem(),
                                               calculatedSystemTime));
                                               */
    options.start_system_timestamp = Tracer::SystemClock::now();
    span.reset(static_cast<Span*>(
        tracer
            ->StartSpanWithOptions("test-span-with-default-steady-timestamp",
                                   options)
            .release()));
    const auto calculatedSteadyTime =
        static_cast<Tracer::SteadyClock::time_point>(
            opentracing::convert_time_point<Tracer::SteadyClock>(
                span->startTimeSystem()));
    /*
    TODO: https://github.com/jaegertracing/jaeger-client-cpp/issues/167
    ASSERT_GE(std::chrono::milliseconds(10),
              absTimeDiff<Tracer::SteadyClock>(span->startTimeSteady(),
                                               calculatedSteadyTime));

*/ options.start_system_timestamp = Tracer::SystemClock::now();
    options.start_steady_timestamp = Tracer::SteadyClock::now();
    span.reset(static_cast<Span*>(
        tracer->StartSpanWithOptions("test-span-with-both-timestamps", options)
            .release()));
    // TODO: https://github.com/jaegertracing/jaeger-client-cpp/issues/167
    
    // ASSERT_EQ(options.start_system_timestamp, span->startTimeSystem());
    // ASSERT_EQ(options.start_steady_timestamp, span->startTimeSteady());

    span.reset();

    // TODO: https://github.com/jaegertracing/jaeger-client-cpp/issues/127

    // There is a problem here. After replacing the global tracer, the spans
    // produced hold the last references to the tracer. After they are
    // published, the tracer is destroyed. This is not the right moment to close
    // the tracer.
    tracer->Close();
    opentracing::Tracer::InitGlobal(opentracing::MakeNoopTracer());
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}

TEST(Tracer, testDebugSpan)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer =
        std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());

    // we force span sampling and require debug-id as the extracted
    // jaeger-debug-id correlation value.
    std::string correlationID = "debug-id";

    StrMap headers ={
        {kJaegerDebugHeader, correlationID},
    };

    WriterMock<opentracing::HTTPHeadersWriter> headerWriter(headers);
    ReaderMock<opentracing::HTTPHeadersReader> headerReader(headers);

    const FakeSpanContext fakeCtx;
    tracer->Inject(fakeCtx, headerWriter);
    auto ext = tracer->Extract(headerReader);

    const SpanContext* ctx = dynamic_cast<SpanContext*>(ext->release());
    opentracing::StartSpanOptions opts;
    opts.references.emplace_back(opentracing::SpanReferenceType::ChildOfRef, ctx);

    auto otspan = tracer->StartSpanWithOptions("name", opts);
    // downcast to jaegertracing span implementation so we can inspect tags
    const std::unique_ptr<Span> span(dynamic_cast<Span*>(otspan.release()));

    const auto& spanTags = span->tags();
    auto spanItr =
        std::find_if(std::begin(spanTags), std::end(spanTags), [](const Tag& tag) {
            return tag.key() == kJaegerDebugHeader;
        });
    ASSERT_NE(std::end(spanTags), spanItr);
    ASSERT_TRUE(spanItr->value().is<std::string>());
    ASSERT_EQ(spanItr->value().get<std::string>(), correlationID);

    tracer->Close();
    opentracing::Tracer::InitGlobal(opentracing::MakeNoopTracer());
}

TEST(Tracer, testConstructorFailure)
{
    Config config;
    ASSERT_THROW(Tracer::make("", config), std::invalid_argument);
}

TEST(Tracer, testDisabledConfig)
{
    Config config(true,
                  false,
                  samplers::Config(),
                  reporters::Config(),
                  propagation::HeadersConfig(),
                  baggage::RestrictionsConfig());
    ASSERT_FALSE(static_cast<bool>(std::dynamic_pointer_cast<Tracer>(
        Tracer::make("test-service", config))));
}

TEST(Tracer, testPropagation)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer =
        std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());
    const std::unique_ptr<Span> span(static_cast<Span*>(
        tracer->StartSpanWithOptions("test-inject", {}).release()));
    span->SetBaggageItem("test-baggage-item-key", "test baggage item value");

    // Binary
    {
        std::stringstream ss;
        ASSERT_TRUE(static_cast<bool>(tracer->Inject(span->context(), ss)));
        auto result = tracer->Extract(ss);
        ASSERT_TRUE(static_cast<bool>(result));
        std::unique_ptr<const SpanContext> extractedCtx(
            static_cast<SpanContext*>(result->release()));
        ASSERT_TRUE(static_cast<bool>(extractedCtx));
        ASSERT_EQ(span->context(), *extractedCtx);
        FakeSpanContext fakeCtx;
        ss.clear();
        ss.str("");
        ASSERT_FALSE(static_cast<bool>(tracer->Inject(fakeCtx, ss)));
    }

    // Text map
    {
        StrMap textMap;
        WriterMock<opentracing::TextMapWriter> textWriter(textMap);
        ASSERT_TRUE(
            static_cast<bool>(tracer->Inject(span->context(), textWriter)));
        ASSERT_EQ(2, textMap.size());
        std::ostringstream oss;
        oss << span->context();
        ASSERT_EQ(oss.str(), textMap.at(kTraceContextHeaderName));
        ASSERT_EQ("test baggage item value",
                  textMap.at(std::string(kTraceBaggageHeaderPrefix) +
                             "test-baggage-item-key"));
        ReaderMock<opentracing::TextMapReader> textReader(textMap);
        auto result = tracer->Extract(textReader);
        ASSERT_TRUE(static_cast<bool>(result));
        std::unique_ptr<const SpanContext> extractedCtx(
            static_cast<SpanContext*>(result->release()));
        ASSERT_TRUE(static_cast<bool>(extractedCtx));
        ASSERT_EQ(span->context(), *extractedCtx);
    }

    // HTTP map
    {
        StrMap headerMap;
        WriterMock<opentracing::HTTPHeadersWriter> headerWriter(headerMap);
        ASSERT_TRUE(
            static_cast<bool>(tracer->Inject(span->context(), headerWriter)));
        ASSERT_EQ(2, headerMap.size());
        std::ostringstream oss;
        oss << span->context();
        ASSERT_EQ(oss.str(), headerMap.at(kTraceContextHeaderName));
        ASSERT_EQ("test%20baggage%20item%20value",
                  headerMap.at(std::string(kTraceBaggageHeaderPrefix) +
                               "test-baggage-item-key"));
        ReaderMock<opentracing::HTTPHeadersReader> headerReader(headerMap);
        auto result = tracer->Extract(headerReader);
        ASSERT_TRUE(static_cast<bool>(result));
        std::unique_ptr<const SpanContext> extractedCtx(
            static_cast<SpanContext*>(result->release()));
        ASSERT_TRUE(static_cast<bool>(extractedCtx));
        ASSERT_EQ(span->context(), *extractedCtx);

        // Test debug header.
        headerMap.clear();
        headerMap[kJaegerDebugHeader] = "yes";
        tracer->Inject(span->context(), headerWriter);
        result = tracer->Extract(headerReader);
        ASSERT_TRUE(static_cast<bool>(result));
        extractedCtx.reset(static_cast<SpanContext*>(result->release()));
        ASSERT_TRUE(static_cast<bool>(extractedCtx));
        ASSERT_NE(span->context(), *extractedCtx);
        SpanContext ctx(
            span->context().traceID(),
            span->context().spanID(),
            span->context().parentID(),
            span->context().flags() |
                static_cast<unsigned char>(SpanContext::Flag::kDebug),
            span->context().baggage(),
            "yes");
        ASSERT_EQ(ctx, *extractedCtx);

        // Test bad trace context.
        headerMap.clear();
        headerMap[kTraceContextHeaderName] = "12345678";
        result = tracer->Extract(headerReader);
        ASSERT_TRUE(static_cast<bool>(result));
        extractedCtx.reset(static_cast<SpanContext*>(result->release()));
        ASSERT_EQ(nullptr, extractedCtx.get());

        // Test empty map.
        headerMap.clear();
        result = tracer->Extract(headerReader);
        ASSERT_TRUE(static_cast<bool>(result));
        extractedCtx.reset(static_cast<SpanContext*>(result->release()));
        ASSERT_EQ(nullptr, extractedCtx.get());

        // Test alternative baggage format.
        headerMap.clear();
        ctx = SpanContext(span->context().traceID(),
                          span->context().spanID(),
                          span->context().parentID(),
                          span->context().flags(),
                          { { "a", "x" }, { "b", "y" }, { "c", "z" } });
        tracer->Inject(ctx, headerWriter);
        for (auto itr = std::begin(headerMap); itr != std::end(headerMap);) {
            if (itr->first.substr(0, std::strlen(kTraceBaggageHeaderPrefix)) ==
                kTraceBaggageHeaderPrefix) {
                itr = headerMap.erase(itr);
            }
            else {
                ++itr;
            }
        }
        headerMap[kJaegerBaggageHeader] = "a=x,b=y,c=z";
        result = tracer->Extract(headerReader);
        ASSERT_TRUE(static_cast<bool>(result));
        extractedCtx.reset(static_cast<SpanContext*>(result->release()));
        ASSERT_TRUE(static_cast<bool>(extractedCtx));
        ASSERT_EQ(3, extractedCtx->baggage().size());
        ASSERT_EQ(ctx, *extractedCtx);
    }
    tracer->Close();
}

TEST(Tracer, testPropagationWithW3CHeaderAndFormat)
{
    const auto handle =
        testutils::TracerUtil::installGlobalTracerW3CPropagation();
    const auto tracer =
        std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());
    SpanContext spanContext(TraceID(1, 2),
                            3,
                            0,
                            static_cast<unsigned char>(SpanContext::Flag::kSampled),
                            StrMap());

    StrMap headerMap;
    WriterMock<opentracing::HTTPHeadersWriter> headerWriter(headerMap);
    ASSERT_TRUE(static_cast<bool>(tracer->Inject(spanContext, headerWriter)));
    ASSERT_EQ(1, headerMap.size());
    ASSERT_EQ("00-00000000000000010000000000000002-0000000000000003-01",
        headerMap.at(kW3CTraceParentHeaderName));

    ReaderMock<opentracing::HTTPHeadersReader> headerReader(headerMap);
    auto result = tracer->Extract(headerReader);
    ASSERT_TRUE(static_cast<bool>(result));
    std::unique_ptr<const SpanContext> extractedCtx(
        static_cast<SpanContext*>(result->release()));
    ASSERT_TRUE(static_cast<bool>(extractedCtx));
    ASSERT_EQ(spanContext, *extractedCtx);

    tracer->Close();
}

TEST(Tracer, testPropagationWithW3CTraceState)
{
    const auto handle =
        testutils::TracerUtil::installGlobalTracerW3CPropagation();
    const auto tracer =
        std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());
    StrMap headerMap{
        { kW3CTraceParentHeaderName,
          "00-00000000000000000000000000000001-0000000000000001-01" },
        { kW3CTraceStateHeaderName, "foo=bar" }
    };

    ReaderMock<opentracing::HTTPHeadersReader> headerReader(headerMap);
    auto result = tracer->Extract(headerReader);
    std::unique_ptr<const SpanContext> ctx(
        static_cast<SpanContext*>(result->release()));
    ASSERT_EQ("foo=bar", ctx->traceState());

    headerMap.clear();

    WriterMock<opentracing::HTTPHeadersWriter> headerWriter(headerMap);
    tracer->Inject(*ctx, headerWriter);
    ASSERT_EQ(2, headerMap.size());
    ASSERT_EQ("foo=bar", headerMap.at(kW3CTraceStateHeaderName));
}

TEST(Tracer, testTracerTags)
{
    std::vector<Tag> tags;
    tags.emplace_back("hostname", std::string("foobar"));
    tags.emplace_back("my.app.version", std::string("1.2.3"));

    Config config(
        false,
        false,
        samplers::Config(
            "const", 1, "", 0, samplers::Config::Clock::duration()),
        reporters::Config(0, std::chrono::milliseconds(100), false, "", ""),
        propagation::HeadersConfig(),
        baggage::RestrictionsConfig(),
        "test-service",
        tags);

    auto tracer = Tracer::make(config);
    const auto jaegerTracer = std::static_pointer_cast<Tracer>(tracer);

    ASSERT_TRUE(std::find(jaegerTracer->tags().begin(),
                          jaegerTracer->tags().end(),
                          Tag("hostname", std::string("foobar"))) !=
        jaegerTracer->tags().end());

    ASSERT_TRUE(std::find(jaegerTracer->tags().begin(),
                          jaegerTracer->tags().end(),
                          Tag("my.app.version", std::string("1.2.3"))) !=
        jaegerTracer->tags().end());

    ASSERT_EQ(std::string("test-service"), jaegerTracer->serviceName());
}

TEST(Tracer, testTracerSimpleChild)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer = std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());
    {
        auto spanRoot = tracer->StartSpan("test-simple-root");
        ASSERT_TRUE(spanRoot);
        auto spanChild = tracer->StartSpan("test-simple-child",
            { opentracing::ChildOf(&spanRoot->context()) });
        ASSERT_TRUE(spanChild);
    }
    tracer->Close();
}

TEST(Tracer, testTracerSpanSelfRef)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer = std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());
    {
        const jaegertracing::SpanContext spanSelfContext { {1, 2}, 3, 0, 0, jaegertracing::SpanContext::StrMap() };
        auto spanRoot = tracer->StartSpan("test-root-self-ref", {jaegertracing::SelfRef(&spanSelfContext)});
        ASSERT_TRUE(spanRoot);
        auto jaegerSpanRoot = dynamic_cast<jaegertracing::Span&>(*spanRoot.get());
        ASSERT_EQ(jaegerSpanRoot.context().traceID(), jaegertracing::TraceID(1, 2));
        ASSERT_EQ(jaegerSpanRoot.context().spanID(), 3);

        auto spanChild = tracer->StartSpan("test-child-self-ref",
            { opentracing::ChildOf(&spanRoot->context()) });
        ASSERT_TRUE(spanChild);
        auto jaegerSpanChild = dynamic_cast<jaegertracing::Span&>(*spanChild.get());
        ASSERT_EQ(jaegerSpanChild.context().traceID(), jaegertracing::TraceID(1, 2));
        ASSERT_NE(jaegerSpanChild.context().spanID(), 3);
    }
    tracer->Close();
}

TEST(Tracer, testTracerSpanSelfRefWithOtherRefs)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer = std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());
    {
        const jaegertracing::SpanContext spanSelfContext { {1, 2}, 3, 0, 0, jaegertracing::SpanContext::StrMap() };
        auto spanRoot = tracer->StartSpan("test-root-self-ref", {jaegertracing::SelfRef(&spanSelfContext)});
        ASSERT_TRUE(spanRoot);
        auto jaegerSpanRoot = dynamic_cast<jaegertracing::Span&>(*spanRoot.get());
        ASSERT_EQ(jaegerSpanRoot.context().traceID(), jaegertracing::TraceID(1, 2));
        ASSERT_EQ(jaegerSpanRoot.context().spanID(), 3);

        const jaegertracing::SpanContext spanSelfContext2 { {1, 2}, 4, 0, 0, jaegertracing::SpanContext::StrMap() };
        auto spanChild = tracer->StartSpan("test-child-self-ref",
            { opentracing::ChildOf(&spanRoot->context()), jaegertracing::SelfRef(&spanSelfContext2) }
        );
        ASSERT_FALSE(spanChild);
    }
    tracer->Close();
}

TEST(Tracer, testTracerWithTraceId128Bit)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer128Bit();
    const auto tracer = std::static_pointer_cast<Tracer>(opentracing::Tracer::Global());
    {
        auto span = tracer->StartSpan("test-operation");
        ASSERT_TRUE(span);
        auto jaegerSpan = dynamic_cast<jaegertracing::Span&>(*span.get());
        auto traceID = jaegerSpan.context().traceID();
        ASSERT_GT(traceID.high(), 0);
    }
    tracer->close();
}

}  // namespace jaegertracing
