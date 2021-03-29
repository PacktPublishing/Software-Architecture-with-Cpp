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

#ifndef JAEGERTRACING_METRICS_NULLSTATSFACTORY_H
#define JAEGERTRACING_METRICS_NULLSTATSFACTORY_H

#include <memory>
#include <string>
#include <unordered_map>

#include "jaegertracing/metrics/NullCounter.h"
#include "jaegertracing/metrics/NullGauge.h"
#include "jaegertracing/metrics/NullTimer.h"
#include "jaegertracing/metrics/StatsFactory.h"

namespace jaegertracing {
namespace metrics {

class Counter;
class Gauge;
class Timer;

class NullStatsFactory : public StatsFactory {
  public:
    using StatsFactory::createCounter;
    using StatsFactory::createGauge;
    using StatsFactory::createTimer;

    std::unique_ptr<Counter>
    createCounter(const std::string&,
                  const std::unordered_map<std::string, std::string>&) override
    {
        return std::unique_ptr<Counter>(new NullCounter());
    }

    std::unique_ptr<Timer>
    createTimer(const std::string&,
                const std::unordered_map<std::string, std::string>&) override
    {
        return std::unique_ptr<Timer>(new NullTimer());
    }

    std::unique_ptr<Gauge>
    createGauge(const std::string&,
                const std::unordered_map<std::string, std::string>&) override
    {
        return std::unique_ptr<Gauge>(new NullGauge());
    }
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_NULLSTATSFACTORY_H
