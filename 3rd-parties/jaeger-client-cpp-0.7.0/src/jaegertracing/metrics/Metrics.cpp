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

#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/metrics/Counter.h"
#include "jaegertracing/metrics/Gauge.h"
#include "jaegertracing/metrics/NullStatsFactory.h"
#include <iterator>
#include <map>
#include <sstream>
#include <utility>

namespace jaegertracing {
namespace metrics {

std::unique_ptr<Metrics> Metrics::makeNullMetrics()
{
    metrics::NullStatsFactory factory;
    return std::unique_ptr<Metrics>(new Metrics(factory));
}

std::string Metrics::addTagsToMetricName(
    const std::string& name,
    const std::unordered_map<std::string, std::string>& tags)
{
    std::ostringstream buffer;
    buffer << name;
    const std::map<std::string, std::string> orderedMap(std::begin(tags),
                                                        std::end(tags));
    for (auto&& pair : orderedMap) {
        buffer << '.' << pair.first << '=' << pair.second;
    }
    return buffer.str();
}

Metrics::~Metrics() = default;

}  // namespace metrics
}  // namespace jaegertracing
