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

#ifndef JAEGERTRACING_UTILS_YAML_H
#define JAEGERTRACING_UTILS_YAML_H

#include "jaegertracing/Constants.h"

#ifdef JAEGERTRACING_WITH_YAML_CPP

#include <yaml-cpp/yaml.h>

namespace jaegertracing {
namespace utils {
namespace yaml {

template <typename ValueType, typename KeyType, typename DefaultValueType>
ValueType findOrDefault(const YAML::Node& node,
                        const KeyType& key,
                        const DefaultValueType& defaultValue)
{
    const auto valueNode = node[key];
    if (!valueNode.IsDefined()) {
        return defaultValue;
    }
    return valueNode.template as<ValueType>();
}

}  // namespace yaml
}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_WITH_YAML_CPP

#endif  // JAEGERTRACING_UTILS_YAML_H
