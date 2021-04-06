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

#include "jaegertracing/baggage/RemoteRestrictionManager.h"

#include <sstream>

#include "jaegertracing/baggage/RemoteRestrictionJSON.h"
#include "jaegertracing/net/http/Response.h"
#include "jaegertracing/utils/ErrorUtil.h"

namespace jaegertracing {
namespace baggage {
namespace {

constexpr auto kDefaultHostPort = "127.0.0.1:5778";

}  // anonymous namespace

RemoteRestrictionManager::RemoteRestrictionManager(
    const std::string& serviceName,
    const std::string& hostPort,
    bool denyBaggageOnInitializationFailure,
    const Clock::duration& refreshInterval,
    logging::Logger& logger,
    metrics::Metrics& metrics)
    : _serviceName(serviceName)
    , _serverAddress(
          net::IPAddress::v4(hostPort.empty() ? kDefaultHostPort : hostPort))
    , _denyBaggageOnInitializationFailure(denyBaggageOnInitializationFailure)
    , _refreshInterval(refreshInterval == Clock::duration()
                           ? defaultRefreshInterval()
                           : refreshInterval)
    , _logger(logger)
    , _metrics(metrics)
    , _running(true)
    , _initialized(false)
    , _thread([this]() { poll(); })
{
}

Restriction
RemoteRestrictionManager::getRestriction(const std::string& /* service */,
                                         const std::string& key)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_initialized) {
        if (_denyBaggageOnInitializationFailure) {
            return Restriction(false, 0);
        }
        return Restriction(true, kDefaultMaxValueLength);
    }

    auto itr = _restrictions.find(key);
    if (itr != std::end(_restrictions)) {
        return itr->second;
    }
    return Restriction(false, 0);
}

void RemoteRestrictionManager::close() noexcept
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (!_running) {
        return;
    }
    _running = false;
    lock.unlock();
    _cv.notify_one();
    _thread.join();
}

void RemoteRestrictionManager::poll() noexcept
{
    net::URI remoteURI;
    try {
        std::ostringstream oss;
        oss << "http://" << _serverAddress.authority()
            << "/baggageRestrictions?service="
            << net::URI::queryEscape(_serviceName);
        remoteURI = net::URI::parse(oss.str());
        updateRestrictions(remoteURI);
    } catch (...) {
        auto logger = logging::consoleLogger();
        utils::ErrorUtil::logError(*logger,
                                   "Failed in RemoteRestrictionManager::poll");
        return;
    }

    Clock::time_point lastUpdateTime = Clock::now();
    while (true) {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait_until(lock, lastUpdateTime + _refreshInterval, [this]() {
                return !_running;
            });
            if (!_running) {
                return;
            }
        }

        if ((Clock::now() - lastUpdateTime) >= _refreshInterval) {
            updateRestrictions(remoteURI);
            lastUpdateTime = Clock::now();
        }
    }
}

void RemoteRestrictionManager::updateRestrictions(
    const net::URI& remoteURI) noexcept
{
    try {
        const auto responseHTTP = net::http::get(remoteURI);
        if (responseHTTP.statusCode() != 200) {
            std::ostringstream oss;
            oss << "Received HTTP error response"
                << ", uri=" << remoteURI
                << ", statusCode=" << responseHTTP.statusCode()
                << ", reason=" << responseHTTP.reason();
            _logger.error(oss.str());
            return;
        }

        thrift::BaggageRestrictionManager_getBaggageRestrictions_result
            response = nlohmann::json::parse(responseHTTP.body());
        if (response.__isset.success) {
            KeyRestrictionMap restrictions;
            restrictions.reserve(response.success.size());
            std::transform(
                std::begin(response.success),
                std::end(response.success),
                std::inserter(restrictions, std::end(restrictions)),
                [](const thrift::BaggageRestriction restriction) {
                    return std::make_pair(
                        restriction.baggageKey,
                        Restriction(true, restriction.maxValueLength));
                });
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _restrictions = std::move(restrictions);
                if (!_initialized) {
                    _initialized = true;
                }
            }
            _metrics.baggageRestrictionsUpdateSuccess().inc(1);
        }
        else {
            std::ostringstream oss;
            oss << "Failed to update baggage restrictions"
                   ", response="
                << responseHTTP.body();
            _logger.error(oss.str());
            _metrics.baggageRestrictionsUpdateFailure().inc(1);
        }
    } catch (...) {
        utils::ErrorUtil::logError(_logger,
                                   "Failed to update baggage restrictions");
        _metrics.baggageRestrictionsUpdateFailure().inc(1);
    }
}

}  // namespace baggage
}  // namespace jaegertracing
