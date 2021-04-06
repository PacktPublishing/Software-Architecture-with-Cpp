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

#include "jaegertracing/samplers/AdaptiveSampler.h"
#include "jaegertracing/samplers/GuaranteedThroughputProbabilisticSampler.h"
#include "jaegertracing/thrift-gen/sampling_types.h"
#include <cassert>
#include <iterator>
#include <memory>
#include <utility>

namespace jaegertracing {
class TraceID;
}  // namespace jaegertracing

namespace jaegertracing {
namespace samplers {
namespace {

AdaptiveSampler::SamplerMap samplersFromStrategies(
    const sampling_manager::thrift::PerOperationSamplingStrategies& strategies)
{
    AdaptiveSampler::SamplerMap samplers;
    for (auto&& strategy : strategies.perOperationStrategies) {
        samplers[strategy.operation] =
            std::make_shared<GuaranteedThroughputProbabilisticSampler>(
                strategies.defaultLowerBoundTracesPerSecond,
                strategy.probabilisticSampling.samplingRate);
    }
    return samplers;
}

}  // anonymous namespace

AdaptiveSampler::AdaptiveSampler(
    const sampling_manager::thrift::PerOperationSamplingStrategies& strategies,
    size_t maxOperations)
    : _samplers(samplersFromStrategies(strategies))
    , _defaultSampler(strategies.defaultSamplingProbability)
    , _lowerBound(strategies.defaultLowerBoundTracesPerSecond)
    , _maxOperations(maxOperations)
    , _mutex()
{
}

SamplingStatus AdaptiveSampler::isSampled(const TraceID& id,
                                          const std::string& operation)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto samplerItr = _samplers.find(operation);
    if (samplerItr != std::end(_samplers)) {
        return samplerItr->second->isSampled(id, operation);
    }
    if (_samplers.size() >= _maxOperations) {
        return _defaultSampler.isSampled(id, operation);
    }

    auto newSampler =
        std::make_shared<GuaranteedThroughputProbabilisticSampler>(
            _lowerBound, _defaultSampler.samplingRate());
    _samplers[operation] = newSampler;
    return newSampler->isSampled(id, operation);
}

void AdaptiveSampler::close()
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto&& pair : _samplers) {
        pair.second->close();
    }
}

void AdaptiveSampler::update(const PerOperationSamplingStrategies& strategies)
{
    const auto lowerBound = strategies.defaultLowerBoundTracesPerSecond;
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto&& strategy : strategies.perOperationStrategies) {
        auto& sampler = _samplers[strategy.operation];
        const auto samplingRate = strategy.probabilisticSampling.samplingRate;
        if (sampler) {
            sampler->update(lowerBound, samplingRate);
        }
        else {
            sampler =
                std::make_shared<GuaranteedThroughputProbabilisticSampler>(
                    lowerBound, samplingRate);
        }
        assert(sampler);
    }
}

}  // namespace samplers
}  // namespace jaegertracing
