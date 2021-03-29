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

#include "jaegertracing/metrics/StatsFactoryImpl.h"
#include "jaegertracing/metrics/Counter.h"
#include "jaegertracing/metrics/Gauge.h"
#include "jaegertracing/metrics/Metric.h"
#include "jaegertracing/metrics/StatsReporter.h"
#include "jaegertracing/metrics/Timer.h"
#include <cstdint>

namespace jaegertracing {
namespace metrics {
namespace {

class ReportedMetric : public Metric {
  public:
    ReportedMetric(StatsReporter& reporter,
                   const std::string& name,
                   const TagMap& tags)
        : Metric(name, tags)
        , _reporter(reporter)
    {
    }

    virtual ~ReportedMetric() = default;

  protected:
    StatsReporter& reporter() { return _reporter; }

  private:
    StatsReporter& _reporter;
};

class CounterImpl : public ReportedMetric, public Counter {
  public:
    CounterImpl(StatsReporter& reporter,
                const std::string& name,
                const std::unordered_map<std::string, std::string>& tags)
        : ReportedMetric(reporter, name, tags)
    {
    }

    void inc(int64_t delta) override
    {
        reporter().incCounter(name(), delta, tags());
    }
};

class TimerImpl : public ReportedMetric, public Timer {
  public:
    TimerImpl(StatsReporter& reporter,
              const std::string& name,
              const std::unordered_map<std::string, std::string>& tags)
        : ReportedMetric(reporter, name, tags)
    {
    }

    void record(int64_t time) override
    {
        reporter().recordTimer(name(), time, tags());
    }
};

class GaugeImpl : public ReportedMetric, public Gauge {
  public:
    GaugeImpl(StatsReporter& reporter,
              const std::string& name,
              const std::unordered_map<std::string, std::string>& tags)
        : ReportedMetric(reporter, name, tags)
    {
    }

    void update(int64_t amount) override
    {
        reporter().updateGauge(name(), amount, tags());
    }
};

}  // anonymous namespace

StatsFactoryImpl::StatsFactoryImpl(StatsReporter& reporter)
    : _reporter(reporter)
{
}

std::unique_ptr<Counter> StatsFactoryImpl::createCounter(
    const std::string& name,
    const std::unordered_map<std::string, std::string>& tags)
{
    return std::unique_ptr<Counter>(new CounterImpl(_reporter, name, tags));
}

std::unique_ptr<Timer> StatsFactoryImpl::createTimer(
    const std::string& name,
    const std::unordered_map<std::string, std::string>& tags)
{
    return std::unique_ptr<Timer>(new TimerImpl(_reporter, name, tags));
}

std::unique_ptr<Gauge> StatsFactoryImpl::createGauge(
    const std::string& name,
    const std::unordered_map<std::string, std::string>& tags)
{
    return std::unique_ptr<Gauge>(new GaugeImpl(_reporter, name, tags));
}

}  // namespace metrics
}  // namespace jaegertracing
