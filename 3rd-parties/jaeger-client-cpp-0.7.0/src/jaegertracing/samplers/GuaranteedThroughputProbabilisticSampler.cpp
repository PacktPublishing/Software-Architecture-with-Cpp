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

#include "jaegertracing/samplers/GuaranteedThroughputProbabilisticSampler.h"

namespace jaegertracing {
namespace samplers {

void GuaranteedThroughputProbabilisticSampler::update(double lowerBound,
                                                      double samplingRate)
{
    if (_samplingRate != samplingRate) {
        _probabilisticSampler = ProbabilisticSampler(samplingRate);
        _samplingRate = _probabilisticSampler.samplingRate();
        _tags = { { kSamplerTypeTagKey, kSamplerTypeLowerBound },
                  { kSamplerParamTagKey, _samplingRate } };
    }

    if (_lowerBound != lowerBound) {
        _lowerBoundSampler.reset(new RateLimitingSampler(lowerBound));
        _lowerBound = lowerBound;
    }
}

}  // namespace samplers
}  // namespace jaegertracing
