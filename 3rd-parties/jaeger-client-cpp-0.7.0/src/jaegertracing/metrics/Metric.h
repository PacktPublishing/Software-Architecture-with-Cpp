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

#ifndef JAEGERTRACING_METRICS_METRIC_H
#define JAEGERTRACING_METRICS_METRIC_H

#include <string>
#include <unordered_map>

#include "jaegertracing/Compilers.h"

namespace jaegertracing {
namespace metrics {

class Metric {
  public:
    using TagMap = std::unordered_map<std::string, std::string>;

    Metric(const std::string& name, const TagMap& tags)
        : _name(name)
        , _tags(tags)
    {
    }

    virtual ~Metric() = default;

    const std::string& name() const { return _name; }

    const TagMap& tags() const { return _tags; }

  protected:
    std::string& name() { return _name; }

    TagMap& tags() { return _tags; }

  private:
    std::string _name;
    TagMap _tags;
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_METRIC_H
