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

#ifndef JAEGERTRACING_BAGGAGE_REMOTERESTRICTIONMANAGER_H
#define JAEGERTRACING_BAGGAGE_REMOTERESTRICTIONMANAGER_H

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/Logging.h"
#include "jaegertracing/baggage/Restriction.h"
#include "jaegertracing/baggage/RestrictionManager.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/net/URI.h"
#include "jaegertracing/thrift-gen/BaggageRestrictionManager.h"

namespace jaegertracing {
namespace baggage {

class RemoteRestrictionManager : public RestrictionManager {
  public:
    using Clock = std::chrono::steady_clock;
    using KeyRestrictionMap = std::unordered_map<std::string, Restriction>;

    static constexpr auto kDefaultDenyBaggageOnInitializationFailure = false;
    static constexpr auto kDefaultMaxValueLength =
        DefaultRestrictionManager::kDefaultMaxValueLength;

    static Clock::duration defaultRefreshInterval()
    {
        return std::chrono::minutes(1);
    }

    RemoteRestrictionManager(const std::string& serviceName,
                             const std::string& hostPort,
                             bool denyBaggageOnInitializationFailure,
                             const Clock::duration& refreshInterval,
                             logging::Logger& logger,
                             metrics::Metrics& metrics);

    ~RemoteRestrictionManager() { close(); }

    Restriction getRestriction(const std::string& /* service */,
                               const std::string& key) override;

    void close() noexcept override;

    const Clock::duration& refreshInterval() const { return _refreshInterval; }

  private:
    void poll() noexcept;

    void updateRestrictions(const net::URI& remoteURI) noexcept;

    std::string _serviceName;
    net::IPAddress _serverAddress;
    bool _denyBaggageOnInitializationFailure;
    Clock::duration _refreshInterval;
    logging::Logger& _logger;
    metrics::Metrics& _metrics;
    KeyRestrictionMap _restrictions;
    bool _running;
    bool _initialized;
    std::thread _thread;
    std::condition_variable _cv;
    std::mutex _mutex;
};

}  // namespace baggage
}  // namespace jaegertracing

#endif  // JAEGERTRACING_BAGGAGE_REMOTERESTRICTIONMANAGER_H
