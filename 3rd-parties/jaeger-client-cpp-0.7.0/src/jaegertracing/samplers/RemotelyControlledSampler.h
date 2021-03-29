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

#ifndef JAEGERTRACING_SAMPLERS_REMOTELYCONTROLLEDSAMPLER_H
#define JAEGERTRACING_SAMPLERS_REMOTELYCONTROLLEDSAMPLER_H

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/Constants.h"
#include "jaegertracing/Logging.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/samplers/ProbabilisticSampler.h"
#include "jaegertracing/samplers/Sampler.h"

namespace jaegertracing {

namespace sampling_manager {
namespace thrift {
class PerOperationSamplingStrategies;
class SamplingStrategyResponse;
class SamplingManagerIf;
}  // namespace thrift
}  // namespace sampling_manager

namespace samplers {

class RemotelyControlledSampler : public Sampler {
  public:
    using Clock = std::chrono::steady_clock;

    RemotelyControlledSampler(const std::string& serviceName,
                              const std::string& samplingServerURL,
                              const std::shared_ptr<Sampler>& sampler,
                              int maxOperations,
                              const Clock::duration& samplingRefreshInterval,
                              logging::Logger& logger,
                              metrics::Metrics& metrics);

    ~RemotelyControlledSampler() { close(); }

    SamplingStatus isSampled(const TraceID& id,
                             const std::string& operation) override;

    void close() override;

    Type type() const override { return Type::kRemotelyControlledSampler; }

  private:
    using PerOperationSamplingStrategies =
        sampling_manager::thrift::PerOperationSamplingStrategies;
    using SamplingStrategyResponse =
        sampling_manager::thrift::SamplingStrategyResponse;

    void pollController();

    void updateSampler();

    void
    updateAdaptiveSampler(const PerOperationSamplingStrategies& strategies);

    void updateRateLimitingOrProbabilisticSampler(
        const SamplingStrategyResponse& response);

    std::string _serviceName;
    std::string _samplingServerURL;
    std::shared_ptr<Sampler> _sampler;
    int _maxOperations;
    Clock::duration _samplingRefreshInterval;
    logging::Logger& _logger;
    metrics::Metrics& _metrics;
    std::shared_ptr<sampling_manager::thrift::SamplingManagerIf> _manager;
    bool _running;
    std::mutex _mutex;
    std::condition_variable _shutdownCV;
    std::thread _thread;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_REMOTELYCONTROLLEDSAMPLER_H
