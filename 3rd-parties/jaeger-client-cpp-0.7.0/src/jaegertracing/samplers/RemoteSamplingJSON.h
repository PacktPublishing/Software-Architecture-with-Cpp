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

#ifndef JAEGERTRACING_SAMPLERS_REMOTESAMPLINGJSON_H
#define JAEGERTRACING_SAMPLERS_REMOTESAMPLINGJSON_H

#include "jaegertracing/Compilers.h"

#include "jaegertracing/thrift-gen/sampling_types.h"
#include <cstdint>
#include <iterator>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jaegertracing {
namespace sampling_manager {
namespace thrift {

const std::map<int, const char*>& samplingStrategyType_VALUES_TO_NAMES();

#define JSON_FROM_FIELD(var, field)                                            \
    {                                                                          \
        json[#field] = var.field;                                              \
    }

#define FIELD_FROM_JSON(var, field)                                            \
    {                                                                          \
        var.__set_##field(json.at(#field));                                    \
    }

inline void to_json(nlohmann::json& json,
                    const SamplingStrategyType::type& type)
{
    json = samplingStrategyType_VALUES_TO_NAMES().at(static_cast<int>(type));
}

inline void from_json(const nlohmann::json& json,
                      SamplingStrategyType::type& type)
{
    const auto str = json.get<std::string>();
    if (str == "PROBABILISTIC") {
        type = SamplingStrategyType::PROBABILISTIC;
        return;
    }
    if (str == "RATE_LIMITING") {
        type = SamplingStrategyType::RATE_LIMITING;
        return;
    }
    std::ostringstream oss;
    oss << "Invalid sampling strategy type " << str;
    throw std::invalid_argument(oss.str());
}

inline void to_json(nlohmann::json& json,
                    const ProbabilisticSamplingStrategy& strategy)
{
    JSON_FROM_FIELD(strategy, samplingRate);
}

inline void from_json(const nlohmann::json& json,
                      ProbabilisticSamplingStrategy& strategy)
{
    FIELD_FROM_JSON(strategy, samplingRate);
}

inline void to_json(nlohmann::json& json,
                    const RateLimitingSamplingStrategy& strategy)
{
    JSON_FROM_FIELD(strategy, maxTracesPerSecond);
}

inline void from_json(const nlohmann::json& json,
                      RateLimitingSamplingStrategy& strategy)
{
    FIELD_FROM_JSON(strategy, maxTracesPerSecond);
}

inline void to_json(nlohmann::json& json,
                    const OperationSamplingStrategy& strategy)
{
    JSON_FROM_FIELD(strategy, operation);
    JSON_FROM_FIELD(strategy, probabilisticSampling);
}

inline void from_json(const nlohmann::json& json,
                      OperationSamplingStrategy& strategy)
{
    FIELD_FROM_JSON(strategy, operation);
    FIELD_FROM_JSON(strategy, probabilisticSampling);
}

inline void to_json(nlohmann::json& json,
                    const PerOperationSamplingStrategies& strategies)
{
    JSON_FROM_FIELD(strategies, defaultSamplingProbability);
    JSON_FROM_FIELD(strategies, defaultLowerBoundTracesPerSecond);
    JSON_FROM_FIELD(strategies, perOperationStrategies);
    if (strategies.__isset.defaultUpperBoundTracesPerSecond) {
        JSON_FROM_FIELD(strategies, defaultUpperBoundTracesPerSecond);
    }
}

inline void from_json(const nlohmann::json& json,
                      PerOperationSamplingStrategies& strategies)
{
    FIELD_FROM_JSON(strategies, defaultSamplingProbability);
    FIELD_FROM_JSON(strategies, defaultLowerBoundTracesPerSecond);
    FIELD_FROM_JSON(strategies, perOperationStrategies);
    auto itr = json.find("defaultUpperBoundTracesPerSecond");
    if (itr != std::end(json)) {
        strategies.__set_defaultUpperBoundTracesPerSecond(itr->get<double>());
    }
}

inline void to_json(nlohmann::json& json,
                    const SamplingStrategyResponse& response)
{
    JSON_FROM_FIELD(response, strategyType);
    if (response.__isset.probabilisticSampling) {
        JSON_FROM_FIELD(response, probabilisticSampling);
    }
    if (response.__isset.rateLimitingSampling) {
        JSON_FROM_FIELD(response, rateLimitingSampling);
    }
    if (response.__isset.operationSampling) {
        JSON_FROM_FIELD(response, operationSampling);
    }
}

inline void from_json(const nlohmann::json& json,
                      SamplingStrategyResponse& response)
{
    FIELD_FROM_JSON(response, strategyType);
    auto itr = json.find("probabilisticSampling");
    if (itr != std::end(json)) {
        response.__set_probabilisticSampling(
            itr->get<ProbabilisticSamplingStrategy>());
    }
    itr = json.find("rateLimitingSampling");
    if (itr != std::end(json)) {
        response.__set_rateLimitingSampling(
            itr->get<RateLimitingSamplingStrategy>());
    }
    itr = json.find("operationSampling");
    if (itr != std::end(json)) {
        response.__set_operationSampling(
            itr->get<PerOperationSamplingStrategies>());
    }
}

#undef FIELD_FROM_JSON
#undef JSON_FROM_FIELD

}  // namespace thrift
}  // namespace sampling_manager
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_REMOTESAMPLINGJSON_H
