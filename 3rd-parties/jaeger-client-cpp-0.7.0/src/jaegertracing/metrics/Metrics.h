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

#ifndef JAEGERTRACING_METRICS_METRICS_H
#define JAEGERTRACING_METRICS_METRICS_H

#include <string>
#include <unordered_map>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/metrics/Counter.h"
#include "jaegertracing/metrics/Gauge.h"
#include "jaegertracing/metrics/StatsFactory.h"
#include "jaegertracing/metrics/StatsFactoryImpl.h"
#include "jaegertracing/metrics/StatsReporter.h"

namespace jaegertracing {
namespace metrics {

class Metrics {
  public:
    static std::unique_ptr<Metrics> makeNullMetrics();

    static std::unique_ptr<Metrics> fromStatsReporter(StatsReporter& reporter)
    {
        // Factory only used for constructor, so need not live past the
        // initialization of Metrics object.
        StatsFactoryImpl factory(reporter);
        return std::unique_ptr<Metrics>(new Metrics(factory));
    }

    static std::string addTagsToMetricName(
        const std::string& name,
        const std::unordered_map<std::string, std::string>& tags);

    explicit Metrics(StatsFactory& factory)
        : _tracesStartedSampled(factory.createCounter(
              "jaeger.traces", { { "state", "started" }, { "sampled", "y" } }))
        , _tracesStartedNotSampled(factory.createCounter(
              "jaeger.traces", { { "state", "started" }, { "sampled", "n" } }))
        , _spansStarted(factory.createCounter(
              "jaeger.spans",
              { { "state", "started" }, { "group", "lifecycle" } }))
        , _spansFinished(factory.createCounter(
              "jaeger.spans",
              { { "state", "finished" }, { "group", "lifecycle" } }))
        , _spansSampled(factory.createCounter(
              "jaeger.spans", { { "group", "sampling" }, { "sampled", "y" } }))
        , _spansNotSampled(factory.createCounter(
              "jaeger.spans", { { "group", "sampling" }, { "sampled", "n" } }))
        , _decodingErrors(factory.createCounter("jaeger.decoding-errors"))
        , _reporterSuccess(factory.createCounter("jaeger.reporter-spans",
                                                 { { "state", "success" } }))
        , _reporterFailure(factory.createCounter("jaeger.reporter-spans",
                                                 { { "state", "failure" } }))
        , _reporterDropped(factory.createCounter("jaeger.reporter-spans",
                                                 { { "state", "dropped" } }))
        , _reporterQueueLength(factory.createGauge("jaeger.reporter-queue"))
        , _samplerRetrieved(factory.createCounter("jaeger.sampler",
                                                  { { "state", "retrieved" } }))
        , _samplerUpdated(factory.createCounter("jaeger.sampler",
                                                { { "state", "updated" } }))
        , _samplerUpdateFailure(factory.createCounter(
              "jaeger.sampler",
              { { "state", "failure" }, { "phase", "updating" } }))
        , _samplerQueryFailure(factory.createCounter(
              "jaeger.sampler",
              { { "state", "failure" }, { "phase", "query" } }))
        , _samplerParsingFailure(factory.createCounter(
              "jaeger.sampler",
              { { "state", "failure" }, { "phase", "parsing" } }))
        , _baggageUpdateSuccess(factory.createCounter("jaeger.baggage-update",
                                                      { { "result", "ok" } }))
        , _baggageUpdateFailure(factory.createCounter("jaeger.baggage-update",
                                                      { { "result", "err" } }))
        , _baggageTruncate(factory.createCounter("jaeger.baggage-truncate"))
        , _baggageRestrictionsUpdateSuccess(factory.createCounter(
              "jaeger.baggage-restrictions-update", { { "result", "ok" } }))
        , _baggageRestrictionsUpdateFailure(factory.createCounter(
              "jaeger.baggage-restrictions-update", { { "result", "err" } }))
    {
    }

    ~Metrics();

    const Counter& tracesStartedSampled() const
    {
        return *_tracesStartedSampled;
    }

    Counter& tracesStartedSampled() { return *_tracesStartedSampled; }

    const Counter& tracesStartedNotSampled() const
    {
        return *_tracesStartedNotSampled;
    }

    Counter& tracesStartedNotSampled() { return *_tracesStartedNotSampled; }

    const Counter& spansStarted() const { return *_spansStarted; }

    Counter& spansStarted() { return *_spansStarted; }

    const Counter& spansFinished() const { return *_spansFinished; }

    Counter& spansFinished() { return *_spansFinished; }

    const Counter& spansSampled() const { return *_spansSampled; }

    Counter& spansSampled() { return *_spansSampled; }

    const Counter& spansNotSampled() const { return *_spansNotSampled; }

    Counter& spansNotSampled() { return *_spansNotSampled; }

    const Counter& decodingErrors() const { return *_decodingErrors; }

    Counter& decodingErrors() { return *_decodingErrors; }

    const Counter& reporterSuccess() const { return *_reporterSuccess; }

    Counter& reporterSuccess() { return *_reporterSuccess; }

    const Counter& reporterFailure() const { return *_reporterFailure; }

    Counter& reporterFailure() { return *_reporterFailure; }

    const Counter& reporterDropped() const { return *_reporterDropped; }

    Counter& reporterDropped() { return *_reporterDropped; }

    const Gauge& reporterQueueLength() const { return *_reporterQueueLength; }

    Gauge& reporterQueueLength() { return *_reporterQueueLength; }

    const Counter& samplerRetrieved() const { return *_samplerRetrieved; }

    Counter& samplerRetrieved() { return *_samplerRetrieved; }

    const Counter& samplerUpdated() const { return *_samplerUpdated; }

    Counter& samplerUpdated() { return *_samplerUpdated; }

    const Counter& samplerUpdateFailure() const
    {
        return *_samplerUpdateFailure;
    }

    Counter& samplerUpdateFailure() { return *_samplerUpdateFailure; }

    const Counter& samplerQueryFailure() const { return *_samplerQueryFailure; }

    Counter& samplerQueryFailure() { return *_samplerQueryFailure; }

    const Counter& samplerParsingFailure() const
    {
        return *_samplerParsingFailure;
    }

    Counter& samplerParsingFailure() { return *_samplerParsingFailure; }

    const Counter& baggageUpdateSuccess() const
    {
        return *_baggageUpdateSuccess;
    }

    Counter& baggageUpdateSuccess() { return *_baggageUpdateSuccess; }

    const Counter& baggageUpdateFailure() const
    {
        return *_baggageUpdateFailure;
    }

    Counter& baggageUpdateFailure() { return *_baggageUpdateFailure; }

    const Counter& baggageTruncate() const { return *_baggageTruncate; }

    Counter& baggageTruncate() { return *_baggageTruncate; }

    const Counter& baggageRestrictionsUpdateSuccess() const
    {
        return *_baggageRestrictionsUpdateSuccess;
    }

    Counter& baggageRestrictionsUpdateSuccess()
    {
        return *_baggageRestrictionsUpdateSuccess;
    }

    const Counter& baggageRestrictionsUpdateFailure() const
    {
        return *_baggageRestrictionsUpdateFailure;
    }

    Counter& baggageRestrictionsUpdateFailure()
    {
        return *_baggageRestrictionsUpdateFailure;
    }

  private:
    std::unique_ptr<Counter> _tracesStartedSampled;
    std::unique_ptr<Counter> _tracesStartedNotSampled;
    std::unique_ptr<Counter> _tracesJoinedSampled;
    std::unique_ptr<Counter> _tracesJoinedNotSampled;
    std::unique_ptr<Counter> _spansStarted;
    std::unique_ptr<Counter> _spansFinished;
    std::unique_ptr<Counter> _spansSampled;
    std::unique_ptr<Counter> _spansNotSampled;
    std::unique_ptr<Counter> _decodingErrors;
    std::unique_ptr<Counter> _reporterSuccess;
    std::unique_ptr<Counter> _reporterFailure;
    std::unique_ptr<Counter> _reporterDropped;
    std::unique_ptr<Gauge> _reporterQueueLength;
    std::unique_ptr<Counter> _samplerRetrieved;
    std::unique_ptr<Counter> _samplerUpdated;
    std::unique_ptr<Counter> _samplerUpdateFailure;
    std::unique_ptr<Counter> _samplerQueryFailure;
    std::unique_ptr<Counter> _samplerParsingFailure;
    std::unique_ptr<Counter> _baggageUpdateSuccess;
    std::unique_ptr<Counter> _baggageUpdateFailure;
    std::unique_ptr<Counter> _baggageTruncate;
    std::unique_ptr<Counter> _baggageRestrictionsUpdateSuccess;
    std::unique_ptr<Counter> _baggageRestrictionsUpdateFailure;
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_METRICS_H
