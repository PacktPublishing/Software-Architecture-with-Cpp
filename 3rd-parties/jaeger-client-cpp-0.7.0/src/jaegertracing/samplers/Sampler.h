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

#ifndef JAEGERTRACING_SAMPLERS_SAMPLER_H
#define JAEGERTRACING_SAMPLERS_SAMPLER_H

#include "jaegertracing/Compilers.h"

#include "jaegertracing/TraceID.h"
#include "jaegertracing/samplers/SamplingStatus.h"

namespace jaegertracing {
namespace samplers {

class Sampler {
  public:
    enum class Type {
        kAdaptiveSampler,
        kConstSampler,
        kGuaranteedThroughputProbabilisticSampler,
        kProbabilisticSampler,
        kRateLimitingSampler,
        kRemotelyControlledSampler
    };

    virtual ~Sampler() = default;

    virtual SamplingStatus isSampled(const TraceID& id,
                                     const std::string& operation) = 0;

    virtual void close() = 0;

    virtual Type type() const = 0;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_SAMPLER_H
