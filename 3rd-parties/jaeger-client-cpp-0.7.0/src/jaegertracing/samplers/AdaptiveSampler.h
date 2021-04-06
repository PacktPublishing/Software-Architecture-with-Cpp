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

#ifndef JAEGERTRACING_SAMPLERS_ADAPTIVESAMPLER_H
#define JAEGERTRACING_SAMPLERS_ADAPTIVESAMPLER_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/Constants.h"
#include "jaegertracing/samplers/GuaranteedThroughputProbabilisticSampler.h"
#include "jaegertracing/samplers/ProbabilisticSampler.h"
#include "jaegertracing/samplers/Sampler.h"

namespace jaegertracing {
namespace sampling_manager {
namespace thrift {
class PerOperationSamplingStrategies;
}
}  // namespace sampling_manager

namespace samplers {

class AdaptiveSampler : public Sampler {
  public:
    using PerOperationSamplingStrategies =
        sampling_manager::thrift::PerOperationSamplingStrategies;
    using SamplerMap = std::unordered_map<
        std::string,
        std::shared_ptr<GuaranteedThroughputProbabilisticSampler>>;

    AdaptiveSampler(const PerOperationSamplingStrategies& strategies,
                    size_t maxOperations);

    ~AdaptiveSampler() { close(); }

    SamplingStatus isSampled(const TraceID& id,
                             const std::string& operation) override;

    void close() override;

    void update(const PerOperationSamplingStrategies& strategies);

    Type type() const override { return Type::kAdaptiveSampler; }

  private:
    SamplerMap _samplers;
    ProbabilisticSampler _defaultSampler;
    double _lowerBound;
    size_t _maxOperations;
    std::mutex _mutex;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_ADAPTIVESAMPLER_H
