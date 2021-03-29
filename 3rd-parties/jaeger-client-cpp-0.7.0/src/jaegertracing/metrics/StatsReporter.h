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

#ifndef JAEGERTRACING_METRICS_STATSREPORTER_H
#define JAEGERTRACING_METRICS_STATSREPORTER_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace jaegertracing {
namespace metrics {

class StatsReporter {
  public:
    using TagMap = std::unordered_map<std::string, std::string>;

    virtual ~StatsReporter() = default;

    void incCounter(const std::string& name, int64_t delta)
    {
        incCounter(name, delta, TagMap());
    }

    void recordTimer(const std::string& name, int64_t delta)
    {
        recordTimer(name, delta, TagMap());
    }

    void updateGauge(const std::string& name, int64_t delta)
    {
        updateGauge(name, delta, TagMap());
    }

    virtual void
    incCounter(const std::string& name, int64_t delta, const TagMap& tags) = 0;

    virtual void
    recordTimer(const std::string& name, int64_t time, const TagMap& tags) = 0;

    virtual void updateGauge(const std::string& name,
                             int64_t amount,
                             const TagMap& tags) = 0;
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_STATSREPORTER_H
