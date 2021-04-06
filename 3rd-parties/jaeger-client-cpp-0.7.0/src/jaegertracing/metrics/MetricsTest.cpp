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

#include "jaegertracing/metrics/Counter.h"
#include "jaegertracing/metrics/Gauge.h"
#include "jaegertracing/metrics/InMemoryStatsReporter.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/metrics/StatsFactoryImpl.h"
#include "jaegertracing/metrics/Timer.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <utility>

namespace jaegertracing {
namespace metrics {

class MetricsTest : public ::testing::Test {
  public:
    MetricsTest()
        : _metricsReporter()
        , _metrics(Metrics::fromStatsReporter(_metricsReporter))
    {
    }

  protected:
    InMemoryStatsReporter _metricsReporter;
    std::unique_ptr<Metrics> _metrics;
};

TEST_F(MetricsTest, testCounter)
{
    constexpr auto counterValue = static_cast<int64_t>(3);
    constexpr auto metricName = "jaeger.test-counter";
    StatsFactoryImpl factory(_metricsReporter);
    auto counter = factory.createCounter(metricName);
    counter->inc(counterValue);
    const auto& counters = _metricsReporter.counters();
    ASSERT_EQ(1, counters.size());
    auto itr = counters.find(metricName);
    ASSERT_TRUE(itr != std::end(counters));
    ASSERT_EQ(counterValue, itr->second);
}

TEST_F(MetricsTest, testGauge)
{
    constexpr auto gaugeValue = static_cast<int64_t>(3);
    constexpr auto metricName = "jaeger.test-gauge";
    StatsFactoryImpl factory(_metricsReporter);
    auto gauge = factory.createGauge(metricName);
    gauge->update(gaugeValue);
    const auto& gauges = _metricsReporter.gauges();
    ASSERT_EQ(1, gauges.size());
    auto itr = gauges.find(metricName);
    ASSERT_TRUE(itr != std::end(gauges));
    ASSERT_EQ(gaugeValue, itr->second);
}

TEST_F(MetricsTest, testTimer)
{
    constexpr auto timeValue = static_cast<int64_t>(5);
    constexpr auto metricName = "jaeger.test-timer";
    StatsFactoryImpl factory(_metricsReporter);
    auto timer = factory.createTimer(metricName);
    timer->record(timeValue);
    const auto& timers = _metricsReporter.timers();
    ASSERT_EQ(1, timers.size());
    auto itr = timers.find(metricName);
    ASSERT_TRUE(itr != std::end(timers));
    ASSERT_EQ(timeValue, itr->second);
}

TEST_F(MetricsTest, testReset)
{
    _metrics->tracesStartedSampled().inc(1);
    const auto& counters = _metricsReporter.counters();
    ASSERT_EQ(1, counters.size());

    _metrics->reporterQueueLength().update(1);
    const auto& gauges = _metricsReporter.gauges();
    ASSERT_EQ(1, gauges.size());

    constexpr auto time = static_cast<int64_t>(1);
    constexpr auto timerName = "jaeger.test-timer";
    _metricsReporter.recordTimer(timerName, time);
    const auto& timers = _metricsReporter.timers();
    ASSERT_EQ(1, timers.size());

    _metricsReporter.reset();
    ASSERT_TRUE(counters.empty());
    ASSERT_TRUE(gauges.empty());
    ASSERT_TRUE(timers.empty());
}

}  // namespace metrics
}  // namespace jaegertracing
