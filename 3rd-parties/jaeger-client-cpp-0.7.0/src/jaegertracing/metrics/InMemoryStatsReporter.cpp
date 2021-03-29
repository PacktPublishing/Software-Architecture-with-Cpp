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

#include "jaegertracing/metrics/InMemoryStatsReporter.h"

#include "jaegertracing/metrics/Counter.h"
#include "jaegertracing/metrics/Gauge.h"
#include "jaegertracing/metrics/Metric.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/metrics/Timer.h"

namespace jaegertracing {
namespace metrics {
namespace {

template <typename Function>
void updateMap(InMemoryStatsReporter::ValueMap& map,
               const std::string& name,
               int64_t newValue,
               const std::unordered_map<std::string, std::string>& tags,
               Function f)
{
    const auto metricName = Metrics::addTagsToMetricName(name, tags);
    auto& initialValue = map[metricName];
    initialValue = f(initialValue, newValue);
}

}  // anonymous namespace

void InMemoryStatsReporter::incCounter(
    const std::string& name,
    int64_t delta,
    const std::unordered_map<std::string, std::string>& tags)
{
    updateMap(_counters,
              name,
              delta,
              tags,
              [](int64_t initialValue, int64_t newValue) {
                  return initialValue + newValue;
              });
}

void InMemoryStatsReporter::recordTimer(
    const std::string& name,
    int64_t time,
    const std::unordered_map<std::string, std::string>& tags)
{
    updateMap(
        _timers, name, time, tags, [](int64_t initialValue, int64_t newValue) {
            return initialValue + newValue;
        });
}

void InMemoryStatsReporter::updateGauge(
    const std::string& name,
    int64_t amount,
    const std::unordered_map<std::string, std::string>& tags)
{
    updateMap(_gauges, name, amount, tags, [](int64_t, int64_t newValue) {
        return newValue;
    });
}

void InMemoryStatsReporter::reset()
{
    _counters.clear();
    _gauges.clear();
    _timers.clear();
}

}  // namespace metrics
}  // namespace jaegertracing
