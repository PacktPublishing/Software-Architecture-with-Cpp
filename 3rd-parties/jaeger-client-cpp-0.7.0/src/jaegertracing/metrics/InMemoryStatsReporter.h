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

#ifndef JAEGERTRACING_METRICS_INMEMORYSTATSREPORTER_H
#define JAEGERTRACING_METRICS_INMEMORYSTATSREPORTER_H

#include "jaegertracing/metrics/StatsReporter.h"
#include <cstdint>
#include <string>
#include <unordered_map>

namespace jaegertracing {
namespace metrics {

class InMemoryStatsReporter : public StatsReporter {
  public:
    using ValueMap = std::unordered_map<std::string, int64_t>;

    using StatsReporter::incCounter;
    using StatsReporter::recordTimer;
    using StatsReporter::updateGauge;

    virtual ~InMemoryStatsReporter() = default;

    void incCounter(const std::string& name,
                    int64_t delta,
                    const TagMap& tags) override;

    void recordTimer(const std::string& name,
                     int64_t time,
                     const TagMap& tags) override;

    void updateGauge(const std::string& name,
                     int64_t time,
                     const TagMap& tags) override;

    void reset();

    const ValueMap& counters() const { return _counters; }

    const ValueMap& gauges() const { return _gauges; }

    const ValueMap& timers() const { return _timers; }

  private:
    ValueMap _counters;
    ValueMap _gauges;
    ValueMap _timers;
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_INMEMORYSTATSREPORTER_H
