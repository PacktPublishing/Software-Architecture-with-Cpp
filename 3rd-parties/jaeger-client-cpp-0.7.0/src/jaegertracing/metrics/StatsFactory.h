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

#ifndef JAEGERTRACING_METRICS_STATSFACTORY_H
#define JAEGERTRACING_METRICS_STATSFACTORY_H

#include <memory>
#include <string>
#include <unordered_map>

#include "jaegertracing/Compilers.h"

namespace jaegertracing {
namespace metrics {

class Counter;
class Gauge;
class Timer;

class StatsFactory {
  public:
    using TagMap = std::unordered_map<std::string, std::string>;

    virtual ~StatsFactory() = default;

    std::unique_ptr<Counter> createCounter(const std::string& name);

    std::unique_ptr<Timer> createTimer(const std::string& name);

    std::unique_ptr<Gauge> createGauge(const std::string& name);

    virtual std::unique_ptr<Counter> createCounter(const std::string& name,
                                                   const TagMap& tags) = 0;

    virtual std::unique_ptr<Timer> createTimer(const std::string& name,
                                               const TagMap& tags) = 0;

    virtual std::unique_ptr<Gauge> createGauge(const std::string& name,
                                               const TagMap& tags) = 0;
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_STATSFACTORY_H
