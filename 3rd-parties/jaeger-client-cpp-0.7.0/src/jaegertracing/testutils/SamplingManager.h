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

#ifndef JAEGERTRACING_TESTUTILS_SAMPLINGMANAGER_H
#define JAEGERTRACING_TESTUTILS_SAMPLINGMANAGER_H

#include "jaegertracing/Compilers.h" 

#include "jaegertracing/thrift-gen/SamplingManager.h"
#include "jaegertracing/thrift-gen/sampling_types.h"
#include <iterator>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

namespace jaegertracing {
namespace testutils {

class SamplingManager : public sampling_manager::thrift::SamplingManagerIf {
  public:
    using Response = sampling_manager::thrift::SamplingStrategyResponse;

    void getSamplingStrategy(Response& response,
                             const std::string& service) override
    {
        using ProbabilisticSamplingStrategy =
            sampling_manager::thrift::ProbabilisticSamplingStrategy;
        using SamplingStrategyType =
            sampling_manager::thrift::SamplingStrategyType;

        std::lock_guard<std::mutex> lock(_mutex);
        auto responseItr = _sampling.find(service);
        if (responseItr != std::end(_sampling)) {
            response = responseItr->second;
            return;
        }

        constexpr auto kSamplingRate = 0.01;
        ProbabilisticSamplingStrategy probabilisticSampling;
        probabilisticSampling.__set_samplingRate(kSamplingRate);
        response.__set_strategyType(SamplingStrategyType::PROBABILISTIC);
        response.__set_probabilisticSampling(probabilisticSampling);
    }

    void addSamplingStrategy(const std::string& serviceName,
                             const Response& response)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _sampling[serviceName] = response;
    }

  private:
    using StrategyMap = std::unordered_map<std::string, Response>;

    StrategyMap _sampling;
    std::mutex _mutex;
};

}  // namespace testutils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_TESTUTILS_SAMPLINGMANAGER_H
