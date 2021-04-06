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

#ifndef JAEGERTRACING_BAGGAGE_REMOTERESTRICTIONJSON_H
#define JAEGERTRACING_BAGGAGE_REMOTERESTRICTIONJSON_H

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "jaegertracing/Compilers.h"
#include "jaegertracing/thrift-gen/BaggageRestrictionManager.h"
#include "jaegertracing/thrift-gen/baggage_types.h"

namespace jaegertracing {
namespace thrift {

inline void to_json(nlohmann::json& json, const BaggageRestriction& restriction)
{
    json["baggageKey"] = restriction.baggageKey;
    json["maxValueLength"] = restriction.maxValueLength;
}

inline void from_json(const nlohmann::json& json,
                      BaggageRestriction& restriction)
{
    restriction.__set_baggageKey(json.at("baggageKey"));
    restriction.__set_maxValueLength(json.at("maxValueLength"));
}

using BaggageRestrictionList =
    BaggageRestrictionManager_getBaggageRestrictions_result;

inline void to_json(nlohmann::json& json, const BaggageRestrictionList& list)
{
    json = list.success;
}

inline void from_json(const nlohmann::json& json, BaggageRestrictionList& list)
{
    list.success = json.get<std::vector<BaggageRestriction>>();
    list.__isset.success = true;
}

}  // namespace thrift
}  // namespace jaegertracing

#endif  // JAEGERTRACING_BAGGAGE_REMOTERESTRICTIONJSON_H
