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

#include "jaegertracing/samplers/Config.h"
#include "jaegertracing/utils/EnvVariable.h"

namespace jaegertracing {
namespace samplers {

constexpr double Config::kDefaultSamplingProbability;
constexpr const char* Config::kDefaultSamplingServerURL;

constexpr const char* Config::kJAEGER_SAMPLER_TYPE_ENV_PROP;
constexpr const char* Config::kJAEGER_SAMPLER_PARAM_ENV_PROP;

void Config::fromEnv()
{
    const auto samplerType = utils::EnvVariable::getStringVariable(kJAEGER_SAMPLER_TYPE_ENV_PROP);
    if (!samplerType.empty()) {
        _type = samplerType;
    }

    const auto param = utils::EnvVariable::getStringVariable(kJAEGER_SAMPLER_PARAM_ENV_PROP);
    if (!param.empty()) {
        std::istringstream iss(param);
        double paramVal = 0;
        if (iss >> paramVal) {
            _param = paramVal;
        }
    }
    const auto samplingServerURL = utils::EnvVariable::getStringVariable(kJAEGER_SAMPLING_ENDPOINT_ENV_PROP);
    if (!samplingServerURL.empty()) {
        _samplingServerURL = samplingServerURL;
    }
}

}  // namespace samplers
}  // namespace jaegertracing
