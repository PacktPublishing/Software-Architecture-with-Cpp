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

#ifndef JAEGERTRACING_TRACER_H
#define JAEGERTRACING_TRACER_H

#include <chrono>
#include <memory>
#include <random>
#include <vector>

#include <opentracing/noop.h>
#include <opentracing/tracer.h>

#include "jaegertracing/Compilers.h"
#include "jaegertracing/Config.h"
#include "jaegertracing/Constants.h"
#include "jaegertracing/Logging.h"
#include "jaegertracing/Span.h"
#include "jaegertracing/Tag.h"
#include "jaegertracing/baggage/BaggageSetter.h"
#include "jaegertracing/baggage/RestrictionManager.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/metrics/NullStatsFactory.h"
#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/platform/Hostname.h"
#include "jaegertracing/propagation/Propagator.h"
#include "jaegertracing/reporters/Reporter.h"
#include "jaegertracing/samplers/Sampler.h"
#include "jaegertracing/utils/ErrorUtil.h"

namespace jaegertracing {

class Tracer : public opentracing::Tracer,
               public std::enable_shared_from_this<Tracer> {
  public:
    using SteadyClock = Span::SteadyClock;
    using SystemClock = Span::SystemClock;
    using string_view = opentracing::string_view;

    static constexpr auto kGen128BitOption = 1;

    static std::shared_ptr<opentracing::Tracer> make(const Config& config)
    {
        return make(config.serviceName(),
                    config,
                    std::shared_ptr<logging::Logger>(logging::nullLogger()));
    }

    static std::shared_ptr<opentracing::Tracer>
    make(const std::string& serviceName, const Config& config)
    {
        return make(serviceName,
                    config,
                    std::shared_ptr<logging::Logger>(logging::nullLogger()));
    }

    static std::shared_ptr<opentracing::Tracer>
    make(const std::string& serviceName,
         const Config& config,
         const std::shared_ptr<logging::Logger>& logger)
    {
        metrics::NullStatsFactory factory;
        return make(serviceName, config, logger, factory);
    }
    static std::shared_ptr<opentracing::Tracer>
    make(const std::string& serviceName,
         const Config& config,
         const std::shared_ptr<logging::Logger>& logger,
         metrics::StatsFactory& statsFactory)
    {
        return make(serviceName, config, logger, statsFactory, config.traceId128Bit() ? kGen128BitOption : 0);
    }
    static std::shared_ptr<opentracing::Tracer>
    make(const std::string& serviceName,
         const Config& config,
         const std::shared_ptr<logging::Logger>& logger,
         metrics::StatsFactory& statsFactory,
         int options);

    ~Tracer() { Close(); }

    std::unique_ptr<opentracing::Span>
    StartSpanWithOptions(string_view operationName,
                         const opentracing::StartSpanOptions& options) const
        noexcept override;

    opentracing::expected<void> Inject(const opentracing::SpanContext& ctx,
                                       std::ostream& writer) const override
    {
        const auto* jaegerCtx = dynamic_cast<const SpanContext*>(&ctx);
        if (!jaegerCtx) {
            return opentracing::make_expected_from_error<void>(
                opentracing::invalid_span_context_error);
        }
        _binaryPropagator.inject(*jaegerCtx, writer);
        return opentracing::make_expected();
    }

    opentracing::expected<void>
    Inject(const opentracing::SpanContext& ctx,
           const opentracing::TextMapWriter& writer) const override
    {
        const auto* jaegerCtx = dynamic_cast<const SpanContext*>(&ctx);
        if (!jaegerCtx) {
            return opentracing::make_expected_from_error<void>(
                opentracing::invalid_span_context_error);
        }
        _textPropagator->inject(*jaegerCtx, writer);
        return opentracing::make_expected();
    }

    opentracing::expected<void>
    Inject(const opentracing::SpanContext& ctx,
           const opentracing::HTTPHeadersWriter& writer) const override
    {
        const auto* jaegerCtx = dynamic_cast<const SpanContext*>(&ctx);
        if (!jaegerCtx) {
            return opentracing::make_expected_from_error<void>(
                opentracing::invalid_span_context_error);
        }
        _httpHeaderPropagator->inject(*jaegerCtx, writer);
        return opentracing::make_expected();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
    Extract(std::istream& reader) const override
    {
        const auto spanContext = _binaryPropagator.extract(reader);
        if (spanContext == SpanContext()) {
            return std::unique_ptr<opentracing::SpanContext>();
        }
        return std::unique_ptr<opentracing::SpanContext>(
            new SpanContext(spanContext));
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
    Extract(const opentracing::TextMapReader& reader) const override
    {
        const auto spanContext = _textPropagator->extract(reader);
        if (spanContext == SpanContext()) {
            return std::unique_ptr<opentracing::SpanContext>();
        }
        return std::unique_ptr<opentracing::SpanContext>(
            new SpanContext(spanContext));
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
    Extract(const opentracing::HTTPHeadersReader& reader) const override
    {
        const auto spanContext = _httpHeaderPropagator->extract(reader);
        if (spanContext == SpanContext()) {
            return std::unique_ptr<opentracing::SpanContext>();
        }
        return std::unique_ptr<opentracing::SpanContext>(
            new SpanContext(spanContext));
    }

    void Close() noexcept override
    {
        try {
            _reporter->close();
            _sampler->close();
            _restrictionManager->close();
        } catch (...) {
            utils::ErrorUtil::logError(*_logger,
                                       "Error occurred in Tracer::Close");
        }
    }

    void close() noexcept { Close(); }

    const std::string& serviceName() const { return _serviceName; }

    const std::vector<Tag>& tags() const { return _tags; }

    const baggage::BaggageSetter& baggageSetter() const
    {
        return _baggageSetter;
    }

    void reportSpan(const Span& span) const
    {
        _metrics->spansFinished().inc(1);
        if (span.context().isSampled()) {
            _reporter->report(span);
        }
    }

  private:
    using TextMapPropagator =
        propagation::Propagator<const opentracing::TextMapReader&,
                                const opentracing::TextMapWriter&>;
    using HTTPHeaderPropagator =
        propagation::Propagator<const opentracing::HTTPHeadersReader&,
                                const opentracing::HTTPHeadersWriter&>;

    Tracer(const std::string& serviceName,
           const std::shared_ptr<samplers::Sampler>& sampler,
           const std::shared_ptr<reporters::Reporter>& reporter,
           const std::shared_ptr<logging::Logger>& logger,
           const std::shared_ptr<metrics::Metrics>& metrics,
           const std::shared_ptr<TextMapPropagator> &textPropagator,
           const std::shared_ptr<HTTPHeaderPropagator> &httpHeaderPropagator,
           const std::vector<Tag>& tags,
           int options)
        : _serviceName(serviceName)
        , _hostIPv4(net::IPAddress::localIP(AF_INET))
        , _sampler(sampler)
        , _reporter(reporter)
        , _metrics(metrics)
        , _logger(logger)
        , _randomNumberGenerator()
        , _textPropagator(textPropagator)
        , _httpHeaderPropagator(httpHeaderPropagator)
        , _binaryPropagator(_metrics)
        , _tags()
        , _restrictionManager(new baggage::DefaultRestrictionManager(0))
        , _baggageSetter(*_restrictionManager, *_metrics)
        , _options(options)
    {
        _tags.push_back(Tag(kJaegerClientVersionTagKey, kJaegerClientVersion));

        try {
            _tags.push_back(Tag(kTracerHostnameTagKey, platform::hostname()));
        } catch (const std::system_error&) {
            // Ignore hostname error.
        }

        if (_hostIPv4 == net::IPAddress()) {
            _logger->error("Unable to determine this host's IP address");
        }
        else {
            _tags.push_back(Tag(kTracerIPTagKey, _hostIPv4.host()));
        }

        std::copy(tags.cbegin(), tags.cend(), std::back_inserter(_tags));

        std::random_device device;
        _randomNumberGenerator.seed(device());
    }

    uint64_t randomID() const
    {
        std::lock_guard<std::mutex> lock(_randomMutex);
        auto value = _randomNumberGenerator();
        while (value == 0) {
            value = _randomNumberGenerator();
        }
        return value;
    }

    using OpenTracingTag = std::pair<std::string, opentracing::Value>;

    std::unique_ptr<Span>
    startSpanInternal(const SpanContext& context,
                      const std::string& operationName,
                      const SystemClock::time_point& startTimeSystem,
                      const SteadyClock::time_point& startTimeSteady,
                      const std::vector<Tag>& internalTags,
                      const std::vector<OpenTracingTag>& tags,
                      bool newTrace,
                      const std::vector<Reference>& references) const;

    using OpenTracingRef = std::pair<opentracing::SpanReferenceType,
                                     const opentracing::SpanContext*>;

    struct AnalyzedReferences {
        AnalyzedReferences()
            : _parent(nullptr)
            , _self(nullptr)
            , _references()
        {
        }

        const SpanContext* _parent;
        const SpanContext* _self;
        std::vector<Reference> _references;
    };

    AnalyzedReferences
    analyzeReferences(const std::vector<OpenTracingRef>& references) const;

    std::string _serviceName;
    net::IPAddress _hostIPv4;
    std::shared_ptr<samplers::Sampler> _sampler;
    std::shared_ptr<reporters::Reporter> _reporter;
    std::shared_ptr<metrics::Metrics> _metrics;
    std::shared_ptr<logging::Logger> _logger;
    mutable std::mt19937_64 _randomNumberGenerator;
    mutable std::mutex _randomMutex;
    std::shared_ptr<TextMapPropagator> _textPropagator;
    std::shared_ptr<HTTPHeaderPropagator> _httpHeaderPropagator;
    propagation::BinaryPropagator _binaryPropagator;
    std::vector<Tag> _tags;
    std::unique_ptr<baggage::RestrictionManager> _restrictionManager;
    baggage::BaggageSetter _baggageSetter;
    int _options;
};


// jaegertracing::SelfRef() returns an opentracing::SpanReference which can be passed to Tracer::StartSpan
// to influence the SpanContext of the newly created span. Specifically, the new span inherits the traceID
// and spanID from the passed SELF reference. It can be used to pass externally generated IDs to the tracer,
// with the purpose of recording spans from data generated elsewhere (e.g. from logs), or by augmenting the
// data of the existing span (Jaeger backend will merge multiple instances of the spans with the same IDs).
// Must be the lone reference, can be used only for root spans
opentracing::SpanReference SelfRef(const opentracing::SpanContext* span_context) noexcept;

}  // namespace jaegertracing

#endif  // JAEGERTRACING_TRACER_H
