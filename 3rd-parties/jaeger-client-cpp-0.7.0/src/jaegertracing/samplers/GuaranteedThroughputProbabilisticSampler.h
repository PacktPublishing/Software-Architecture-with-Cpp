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

#ifndef JAEGERTRACING_SAMPLERS_GUARANTEEDTHROUGHPUTPROBABILISTICSAMPLER_H
#define JAEGERTRACING_SAMPLERS_GUARANTEEDTHROUGHPUTPROBABILISTICSAMPLER_H

#include "jaegertracing/Compilers.h"
#include "jaegertracing/Constants.h"
#include "jaegertracing/Tag.h"
#include "jaegertracing/samplers/ProbabilisticSampler.h"
#include "jaegertracing/samplers/RateLimitingSampler.h"
#include "jaegertracing/samplers/Sampler.h"
#include "jaegertracing/samplers/SamplingStatus.h"
#include <memory>
#include <string>
#include <vector>

namespace jaegertracing {
class TraceID;
}  // namespace jaegertracing

namespace jaegertracing {
namespace samplers {

class GuaranteedThroughputProbabilisticSampler : public Sampler
{
  public:
    GuaranteedThroughputProbabilisticSampler(double lowerBound,
                                             double samplingRate)
        : _probabilisticSampler(samplingRate)
        , _samplingRate(_probabilisticSampler.samplingRate())
        , _lowerBoundSampler(new RateLimitingSampler(lowerBound))
        , _lowerBound(lowerBound)
        , _tags({ { kSamplerTypeTagKey, kSamplerTypeLowerBound },
                  { kSamplerParamTagKey, _samplingRate } })
    {
    }

    SamplingStatus isSampled(const TraceID& id,
                             const std::string& operation) override
    {
        const auto samplingStatus =
            _probabilisticSampler.isSampled(id, operation);
        if (samplingStatus.isSampled()) {
            _lowerBoundSampler->isSampled(id, operation);
            return samplingStatus;
        }
        const auto sampled =
            _lowerBoundSampler->isSampled(id, operation).isSampled();
        return SamplingStatus(sampled, _tags);
    }

    void close() override
    {
        _probabilisticSampler.close();
        _lowerBoundSampler->close();
    }

    void update(double lowerBound, double samplingRate);

    double lowerBound() const { return _lowerBound; }

    double samplingRate() const { return _samplingRate; }

    Type type() const override
    {
        return Type::kGuaranteedThroughputProbabilisticSampler;
    }

  private:
    ProbabilisticSampler _probabilisticSampler;
    double _samplingRate;
    std::unique_ptr<RateLimitingSampler> _lowerBoundSampler;
    double _lowerBound;
    std::vector<Tag> _tags;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_GUARANTEEDTHROUGHPUTPROBABILISTICSAMPLER_H
