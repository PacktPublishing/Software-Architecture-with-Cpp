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

#include "jaegertracing/Tag.h"
#include "jaegertracing/Tracer.h"
#include "jaegertracing/Reference.h"
#include "jaegertracing/TraceID.h"
#include "jaegertracing/propagation/JaegerPropagator.h"
#include "jaegertracing/propagation/W3CPropagator.h"
#include "jaegertracing/samplers/SamplingStatus.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <iterator>
#include <opentracing/util.h>
#include <tuple>

namespace jaegertracing {
namespace {

using SystemClock = Tracer::SystemClock;
using SteadyClock = Tracer::SteadyClock;
using TimePoints = std::tuple<SystemClock::time_point, SteadyClock::time_point>;

// An extension of opentracing::SpanReferenceType enum. See jaegertracing::SelfRef().
const static int SpanReferenceType_JaegerSpecific_SelfRef = 99;

TimePoints determineStartTimes(const opentracing::StartSpanOptions& options)
{
    if (options.start_system_timestamp == SystemClock::time_point() &&
        options.start_steady_timestamp == SteadyClock::time_point()) {
        return std::make_tuple(SystemClock::now(), SteadyClock::now());
    }
    if (options.start_system_timestamp == SystemClock::time_point()) {
        return std::make_tuple(opentracing::convert_time_point<SystemClock>(
                                   options.start_steady_timestamp),
                               options.start_steady_timestamp);
    }
    if (options.start_steady_timestamp == SteadyClock::time_point()) {
        return std::make_tuple(options.start_system_timestamp,
                               opentracing::convert_time_point<SteadyClock>(
                                   options.start_system_timestamp));
    }
    return std::make_tuple(options.start_system_timestamp,
                           options.start_steady_timestamp);
}

}  // anonymous namespace

using StrMap = SpanContext::StrMap;

constexpr int Tracer::kGen128BitOption;

std::unique_ptr<opentracing::Span>
Tracer::StartSpanWithOptions(string_view operationName,
                             const opentracing::StartSpanOptions& options) const
    noexcept
{
    try {
        const auto result = analyzeReferences(options.references);
        const auto* parent = result._parent;
        const auto* self = result._self;
        const auto& references = result._references;
        if (self && (parent || !references.empty()))
        {
            throw std::invalid_argument("Self and references are exclusive. Only one of them can be specified");
        }

        std::vector<Tag> samplerTags;
        auto newTrace = false;
        SpanContext ctx;
        if (!parent || !parent->isValid()) {
            newTrace = true;
            auto highID = static_cast<uint64_t>(0);
            auto lowID = static_cast<uint64_t>(0);
            if (self) {
                highID = self->traceID().high();
                lowID = self->traceID().low();
            }
            else {
                if (_options & kGen128BitOption) {
                    highID = randomID();
                }
                lowID = randomID();
            }
            const TraceID traceID(highID, lowID);
            const auto spanID = self ? self->spanID() : traceID.low();
            const auto parentID = 0;
            auto flags = static_cast<unsigned char>(0);
            if (parent && parent->isDebugIDContainerOnly()) {
                flags |=
                    (static_cast<unsigned char>(SpanContext::Flag::kSampled) |
                     static_cast<unsigned char>(SpanContext::Flag::kDebug));
                samplerTags.push_back(Tag(kJaegerDebugHeader, parent->debugID()));
            }
            else {
                const auto samplingStatus =
                    _sampler->isSampled(traceID, operationName);
                if (samplingStatus.isSampled()) {
                    flags |=
                        static_cast<unsigned char>(SpanContext::Flag::kSampled);
                    samplerTags = samplingStatus.tags();
                }
            }
            ctx = SpanContext(traceID, spanID, parentID, flags, StrMap());
        }
        else {
            const auto traceID = parent->traceID();
            const auto spanID = randomID();
            const auto parentID = parent->spanID();
            const auto flags = parent->flags();
            ctx = SpanContext(traceID, spanID, parentID, flags, StrMap());
        }

        if (parent && !parent->baggage().empty()) {
            ctx = ctx.withBaggage(parent->baggage());
        }

        SystemClock::time_point startTimeSystem;
        SteadyClock::time_point startTimeSteady;
        std::tie(startTimeSystem, startTimeSteady) =
            determineStartTimes(options);
        return startSpanInternal(ctx,
                                 operationName,
                                 startTimeSystem,
                                 startTimeSteady,
                                 samplerTags,
                                 options.tags,
                                 newTrace,
                                 references);
    } catch (const std::exception& ex) {
        std::ostringstream oss;
        oss << "Error occurred in Tracer::StartSpanWithOptions: " << ex.what();
        utils::ErrorUtil::logError(*_logger, oss.str());
        return nullptr;
    } catch (...) {
        utils::ErrorUtil::logError(
            *_logger, "Error occurred in Tracer::StartSpanWithOptions");
        return nullptr;
    }
}

std::unique_ptr<Span>
Tracer::startSpanInternal(const SpanContext& context,
                          const std::string& operationName,
                          const SystemClock::time_point& startTimeSystem,
                          const SteadyClock::time_point& startTimeSteady,
                          const std::vector<Tag>& internalTags,
                          const std::vector<OpenTracingTag>& tags,
                          bool newTrace,
                          const std::vector<Reference>& references) const
{
    std::vector<Tag> spanTags;
    spanTags.reserve(tags.size() + internalTags.size());
    std::transform(
        std::begin(tags),
        std::end(tags),
        std::back_inserter(spanTags),
        [](const OpenTracingTag& tag) { return Tag(tag.first, tag.second); });
    spanTags.insert(
        std::end(spanTags), std::begin(internalTags), std::end(internalTags));

    std::unique_ptr<Span> span(new Span(shared_from_this(),
                                        context,
                                        operationName,
                                        startTimeSystem,
                                        startTimeSteady,
                                        spanTags,
                                        references));

    _metrics->spansStarted().inc(1);
    if (span->context().isSampled()) {
        _metrics->spansSampled().inc(1);
        if (newTrace) {
            _metrics->tracesStartedSampled().inc(1);
        }
    }
    else {
        _metrics->spansNotSampled().inc(1);
        if (newTrace) {
            _metrics->tracesStartedNotSampled().inc(1);
        }
    }

    return span;
}

Tracer::AnalyzedReferences
Tracer::analyzeReferences(const std::vector<OpenTracingRef>& references) const
{
    AnalyzedReferences result;
    auto hasParent = false;
    const auto* parent = result._parent;
    for (auto&& ref : references) {
        const auto* ctx = dynamic_cast<const SpanContext*>(ref.second);

        if (!ctx) {
            _logger->error("Reference contains invalid type of SpanReference");
            continue;
        }

        if (!ctx->isValid() && !ctx->isDebugIDContainerOnly() &&
            ctx->baggage().empty()) {
            continue;
        }

        if (static_cast<int>(ref.first) == SpanReferenceType_JaegerSpecific_SelfRef)
        {
            result._self = ctx;
            continue; // not a reference
        }

        result._references.emplace_back(Reference(*ctx, ref.first));

        if (!hasParent) {
            parent = ctx;
            hasParent =
                (ref.first == opentracing::SpanReferenceType::ChildOfRef);
        }
    }

    if (!hasParent && parent && parent->isValid()) {
        // Use `FollowsFromRef` in place of `ChildOfRef`.
        hasParent = true;
    }

    if (hasParent) {
        assert(parent);
        result._parent = parent;
    }

    return result;
}

std::shared_ptr<opentracing::Tracer>
Tracer::make(const std::string& serviceName,
                   const Config& config,
     const std::shared_ptr<logging::Logger>& logger,
     metrics::StatsFactory& statsFactory, int options)
{
    if (serviceName.empty()) {
        throw std::invalid_argument("no service name provided");
    }

    if (config.disabled()) {
        return opentracing::MakeNoopTracer();
    }

    auto metrics = std::make_shared<metrics::Metrics>(statsFactory);

    std::shared_ptr<TextMapPropagator> textPropagator;
    std::shared_ptr<HTTPHeaderPropagator> httpHeaderPropagator;
    if (config.propagationFormat() == propagation::Format::W3C) {
        textPropagator = std::shared_ptr<TextMapPropagator>(
            new propagation::W3CTextMapPropagator(config.headers(), metrics));
        httpHeaderPropagator = std::shared_ptr<HTTPHeaderPropagator>(
            new propagation::W3CHTTPHeaderPropagator(config.headers(),
                                                     metrics));
    }
    else {
        textPropagator = std::shared_ptr<TextMapPropagator>(
            new propagation::JaegerTextMapPropagator(config.headers(),
                                                     metrics));
        httpHeaderPropagator = std::shared_ptr<HTTPHeaderPropagator>(
            new propagation::JaegerHTTPHeaderPropagator(config.headers(),
                                                        metrics));
    }

    std::shared_ptr<samplers::Sampler> sampler(
        config.sampler().makeSampler(serviceName, *logger, *metrics));
    std::shared_ptr<reporters::Reporter> reporter(
        config.reporter().makeReporter(serviceName, *logger, *metrics));
    return std::shared_ptr<Tracer>(new Tracer(serviceName,
                                              sampler,
                                              reporter,
                                              logger,
                                              metrics,
                                              textPropagator,
                                              httpHeaderPropagator,
                                              config.tags(),
                                              options));
}

opentracing::SpanReference SelfRef(const opentracing::SpanContext* span_context) noexcept {
    return {static_cast<opentracing::SpanReferenceType>(SpanReferenceType_JaegerSpecific_SelfRef), span_context};
}

}  // namespace jaegertracing
