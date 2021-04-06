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

#ifndef JAEGERTRACING_REPORTERS_CONFIG_H
#define JAEGERTRACING_REPORTERS_CONFIG_H

#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include "jaegertracing/Logging.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/reporters/Reporter.h"
#include "jaegertracing/utils/YAML.h"
#include "jaegertracing/utils/HTTPTransporter.h"
#include "jaegertracing/utils/UDPTransporter.h"

namespace jaegertracing {
namespace reporters {

class Config {
  public:
    using Clock = std::chrono::steady_clock;

    static constexpr auto kDefaultQueueSize = 100;
    static constexpr auto kDefaultLocalAgentHostPort = "127.0.0.1:6831";
    static constexpr auto kDefaultEndpoint = "";

    static constexpr auto kJAEGER_AGENT_HOST_ENV_PROP = "JAEGER_AGENT_HOST";
    static constexpr auto kJAEGER_AGENT_PORT_ENV_PROP = "JAEGER_AGENT_PORT";
    static constexpr auto kJAEGER_ENDPOINT_ENV_PROP = "JAEGER_ENDPOINT";

    static constexpr auto kJAEGER_REPORTER_LOG_SPANS_ENV_PROP = "JAEGER_REPORTER_LOG_SPANS";
    static constexpr auto kJAEGER_REPORTER_FLUSH_INTERVAL_ENV_PROP = "JAEGER_REPORTER_FLUSH_INTERVAL";
    static constexpr auto kJAEGER_REPORTER_MAX_QUEUE_SIZE_ENV_PROP = "JAEGER_REPORTER_MAX_QUEUE_SIZE";



    static Clock::duration defaultBufferFlushInterval()
    {
        return std::chrono::seconds(10);
    }

#ifdef JAEGERTRACING_WITH_YAML_CPP

    static Config parse(const YAML::Node& configYAML)
    {
        if (!configYAML.IsDefined() || !configYAML.IsMap()) {
            return Config();
        }

        const auto queueSize =
            utils::yaml::findOrDefault<int>(configYAML, "queueSize", 0);
        const auto bufferFlushInterval =
            std::chrono::seconds(utils::yaml::findOrDefault<int>(
                configYAML, "bufferFlushInterval", 0));
        const auto logSpans =
            utils::yaml::findOrDefault<bool>(configYAML, "logSpans", false);
        const auto localAgentHostPort = utils::yaml::findOrDefault<std::string>(
            configYAML, "localAgentHostPort", "");
        const auto endpoint = utils::yaml::findOrDefault<std::string>(
            configYAML, "endpoint", "");
        return Config(
            queueSize, bufferFlushInterval, logSpans, localAgentHostPort, endpoint);
    }

#endif  // JAEGERTRACING_WITH_YAML_CPP

    explicit Config(
        int queueSize = kDefaultQueueSize,
        const Clock::duration& bufferFlushInterval =
            defaultBufferFlushInterval(),
        bool logSpans = false,
        const std::string& localAgentHostPort = kDefaultLocalAgentHostPort, const std::string& endpoint = kDefaultEndpoint)
        : _queueSize(queueSize > 0 ? queueSize : kDefaultQueueSize)
        , _bufferFlushInterval(bufferFlushInterval.count() > 0
                                   ? bufferFlushInterval
                                   : defaultBufferFlushInterval())
        , _logSpans(logSpans)
        , _localAgentHostPort(localAgentHostPort.empty()
                                  ? kDefaultLocalAgentHostPort
                                  : localAgentHostPort)
        , _endpoint(endpoint)
    {
    }

    std::unique_ptr<Reporter> makeReporter(const std::string& serviceName,
                                           logging::Logger& logger,
                                           metrics::Metrics& metrics) const;

    int queueSize() const { return _queueSize; }

    const Clock::duration& bufferFlushInterval() const
    {
        return _bufferFlushInterval;
    }

    bool logSpans() const { return _logSpans; }

    const std::string& localAgentHostPort() const
    {
        return _localAgentHostPort;
    }

    const std::string& endpoint() const
    {
      return _endpoint;
    }

    void fromEnv();

  private:
    int _queueSize;
    Clock::duration _bufferFlushInterval;
    bool _logSpans;
    std::string _localAgentHostPort;
    std::string _endpoint;
};

}  // namespace reporters
}  // namespace jaegertracing

#endif  // JAEGERTRACING_REPORTERS_CONFIG_H
