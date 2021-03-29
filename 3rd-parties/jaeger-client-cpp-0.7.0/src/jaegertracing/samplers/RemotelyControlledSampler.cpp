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

#include "jaegertracing/samplers/RemotelyControlledSampler.h"

#include <cassert>
#include <sstream>

#include "jaegertracing/metrics/Counter.h"
#include "jaegertracing/metrics/Gauge.h"
#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/URI.h"
#include "jaegertracing/net/http/Response.h"
#include "jaegertracing/samplers/AdaptiveSampler.h"
#include "jaegertracing/samplers/RemoteSamplingJSON.h"
#include "jaegertracing/thrift-gen/SamplingManager.h"
#include "jaegertracing/utils/ErrorUtil.h"

namespace jaegertracing {
namespace samplers {
namespace {

class HTTPSamplingManager : public sampling_manager::thrift::SamplingManagerIf {
  public:
    using SamplingStrategyResponse =
        sampling_manager::thrift::SamplingStrategyResponse;

    HTTPSamplingManager(const std::string& serverURL, logging::Logger& logger)
        : _serverURI(net::URI::parse(serverURL))
        , _logger(logger)
    {
        try {
            net::Socket socket;
            socket.open(AF_INET, SOCK_STREAM);
            _serverAddr = socket.connect(serverURL);
        } catch (...) {
            utils::ErrorUtil::logError(_logger, "cannot connect to socket");
        }
    }

    void getSamplingStrategy(SamplingStrategyResponse& result,
                             const std::string& serviceName) override
    {
        if (_serverAddr == net::IPAddress()) {
            return;
        }
        auto uri = _serverURI;
        uri._query = "service=" + net::URI::queryEscape(serviceName);
        const auto responseHTTP = net::http::get(uri);
        if (responseHTTP.statusCode() != 200) {
            std::ostringstream oss;
            oss << "Received HTTP error response"
                   ", uri="
                << uri << ", statusCode=" << responseHTTP.statusCode()
                << ", reason=" << responseHTTP.reason();
            _logger.error(oss.str());
            return;
        }

        result = nlohmann::json::parse(responseHTTP.body());
    }

  private:
    net::URI _serverURI;
    net::IPAddress _serverAddr;
    logging::Logger& _logger;
};

}  // anonymous namespace

RemotelyControlledSampler::RemotelyControlledSampler(
    const std::string& serviceName,
    const std::string& samplingServerURL,
    const std::shared_ptr<Sampler>& sampler,
    int maxOperations,
    const Clock::duration& samplingRefreshInterval,
    logging::Logger& logger,
    metrics::Metrics& metrics)
    : _serviceName(serviceName)
    , _samplingServerURL(samplingServerURL)
    , _sampler(sampler)
    , _maxOperations(maxOperations)
    , _samplingRefreshInterval(samplingRefreshInterval)
    , _logger(logger)
    , _metrics(metrics)
    , _manager(
          std::make_shared<HTTPSamplingManager>(_samplingServerURL, _logger))
    , _running(true)
    , _mutex()
    , _shutdownCV()
    , _thread([this]() { pollController(); })
{
    assert(_sampler);
}

SamplingStatus
RemotelyControlledSampler::isSampled(const TraceID& id,
                                     const std::string& operation)
{
    std::lock_guard<std::mutex> lock(_mutex);
    assert(_sampler);
    return _sampler->isSampled(id, operation);
}

void RemotelyControlledSampler::close()
{
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_running) {
            return;
        }
        _running = false;
        lock.unlock();
        _shutdownCV.notify_one();
    }
    _thread.join();
}

void RemotelyControlledSampler::pollController()
{
    while (_running) {
        updateSampler();
        std::unique_lock<std::mutex> lock(_mutex);
        _shutdownCV.wait_for(
            lock, _samplingRefreshInterval, [this]() { return !_running; });
    }
}

void RemotelyControlledSampler::updateSampler()
{
    assert(_manager);
    sampling_manager::thrift::SamplingStrategyResponse response;
    try {
        assert(_manager);
        _manager->getSamplingStrategy(response, _serviceName);
    } catch (const std::exception& ex) {
        _metrics.samplerQueryFailure().inc(1);
        return;
    } catch (...) {
        _metrics.samplerQueryFailure().inc(1);
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    _metrics.samplerRetrieved().inc(1);

    if (response.__isset.operationSampling) {
        updateAdaptiveSampler(response.operationSampling);
    }
    else {
        try {
            updateRateLimitingOrProbabilisticSampler(response);
        } catch (const std::exception& ex) {
            _metrics.samplerUpdateFailure().inc(1);
            return;
        } catch (...) {
            _metrics.samplerUpdateFailure().inc(1);
            return;
        }
    }
    _metrics.samplerUpdated().inc(1);
}

void RemotelyControlledSampler::updateAdaptiveSampler(
    const PerOperationSamplingStrategies& strategies)
{
    auto sampler = _sampler;
    assert(sampler);
    if (sampler->type() == Type::kAdaptiveSampler) {
        static_cast<AdaptiveSampler&>(*sampler).update(strategies);
    }
    else {
        _sampler =
            std::make_shared<AdaptiveSampler>(strategies, _maxOperations);
    }
}

void RemotelyControlledSampler::updateRateLimitingOrProbabilisticSampler(
    const SamplingStrategyResponse& response)
{
    std::shared_ptr<Sampler> sampler;
    if (response.__isset.probabilisticSampling) {
        sampler = std::make_shared<ProbabilisticSampler>(
            response.probabilisticSampling.samplingRate);
    }
    else if (response.__isset.rateLimitingSampling) {
        sampler = std::make_shared<RateLimitingSampler>(
            response.rateLimitingSampling.maxTracesPerSecond);
    }
    else {
        std::ostringstream oss;
        oss << "Unsupported sampling strategy type " << response.strategyType;
        throw std::runtime_error(oss.str());
    }
    _sampler = sampler;
}

}  // namespace samplers
}  // namespace jaegertracing
