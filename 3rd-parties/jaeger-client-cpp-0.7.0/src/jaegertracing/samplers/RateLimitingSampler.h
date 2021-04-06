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

#ifndef JAEGERTRACING_SAMPLERS_RATELIMITINGSAMPLER_H
#define JAEGERTRACING_SAMPLERS_RATELIMITINGSAMPLER_H

#include "jaegertracing/Compilers.h"

#include "jaegertracing/Constants.h"
#include "jaegertracing/Tag.h"
#include "jaegertracing/samplers/Sampler.h"
#include "jaegertracing/samplers/SamplingStatus.h"
#include "jaegertracing/utils/RateLimiter.h"
#include <algorithm>
#include <string>
#include <vector>

namespace jaegertracing {
class TraceID;
}  // namespace jaegertracing

namespace jaegertracing {
namespace samplers {

class RateLimitingSampler : public Sampler {
  public:
    explicit RateLimitingSampler(double maxTracesPerSecond)
        : _maxTracesPerSecond(maxTracesPerSecond)
        , _rateLimiter(_maxTracesPerSecond, std::max(_maxTracesPerSecond, 1.0))
        , _tags({ { kSamplerTypeTagKey, kSamplerTypeRateLimiting },
                  { kSamplerParamTagKey, maxTracesPerSecond } })
    {
    }

    SamplingStatus isSampled(const TraceID& id,
                             const std::string& operation) override
    {
        return SamplingStatus(_rateLimiter.checkCredit(1), _tags);
    }

    void close() override {}

    Type type() const override { return Type::kRateLimitingSampler; }

  private:
    double _maxTracesPerSecond;
    utils::RateLimiter<> _rateLimiter;
    std::vector<Tag> _tags;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_RATELIMITINGSAMPLER_H
