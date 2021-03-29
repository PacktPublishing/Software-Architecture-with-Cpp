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

#ifndef JAEGERTRACING_SAMPLERS_CONSTSAMPLER_H
#define JAEGERTRACING_SAMPLERS_CONSTSAMPLER_H

#include "jaegertracing/Constants.h"
#include "jaegertracing/Tag.h"
#include "jaegertracing/samplers/Sampler.h"
#include "jaegertracing/samplers/SamplingStatus.h"
#include <string>
#include <vector>

namespace jaegertracing {
class TraceID;
}  // namespace jaegertracing

namespace jaegertracing {
namespace samplers {

class ConstSampler : public Sampler {
  public:
    explicit ConstSampler(bool sample)
        : _decision(sample)
        , _tags({ { kSamplerTypeTagKey, kSamplerTypeConst },
                  { kSamplerParamTagKey, _decision } })
    {
    }

    SamplingStatus isSampled(const TraceID& id,
                             const std::string& operation) override
    {
        return SamplingStatus(_decision, _tags);
    }

    void close() override {}

    Type type() const override { return Type::kConstSampler; }

  private:
    bool _decision;
    std::vector<Tag> _tags;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_CONSTSAMPLER_H
