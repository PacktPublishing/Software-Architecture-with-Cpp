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

#ifndef JAEGERTRACING_METRICS_NULLSTATSREPORTER_H
#define JAEGERTRACING_METRICS_NULLSTATSREPORTER_H

#include "jaegertracing/metrics/StatsReporter.h"
#include <cstdint>
#include <string>
#include <unordered_map>

namespace jaegertracing {
namespace metrics {

class NullStatsReporter : public StatsReporter {
  public:
    using StatsReporter::incCounter;
    using StatsReporter::recordTimer;
    using StatsReporter::updateGauge;

    ~NullStatsReporter() = default;

    void
    incCounter(const std::string&,
               int64_t,
               const std::unordered_map<std::string, std::string>&) override
    {
    }

    void
    recordTimer(const std::string&,
                int64_t,
                const std::unordered_map<std::string, std::string>&) override
    {
    }

    void
    updateGauge(const std::string&,
                int64_t,
                const std::unordered_map<std::string, std::string>&) override
    {
    }
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_NULLSTATSREPORTER_H
