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

#include <algorithm>

#include "jaegertracing/reporters/Config.h"
#include "jaegertracing/ThriftSender.h"
#include "jaegertracing/reporters/CompositeReporter.h"
#include "jaegertracing/reporters/LoggingReporter.h"
#include "jaegertracing/reporters/RemoteReporter.h"
#include "jaegertracing/utils/EnvVariable.h"

namespace jaegertracing {
namespace reporters {

constexpr int Config::kDefaultQueueSize;
constexpr const char* Config::kDefaultLocalAgentHostPort;
constexpr const char* Config::kDefaultEndpoint;
constexpr const char* Config::kJAEGER_AGENT_HOST_ENV_PROP;
constexpr const char* Config::kJAEGER_AGENT_PORT_ENV_PROP;
constexpr const char* Config::kJAEGER_ENDPOINT_ENV_PROP;

constexpr const char* Config::kJAEGER_REPORTER_LOG_SPANS_ENV_PROP;
constexpr const char* Config::kJAEGER_REPORTER_FLUSH_INTERVAL_ENV_PROP;
constexpr const char* Config::kJAEGER_REPORTER_MAX_QUEUE_SIZE_ENV_PROP;

std::unique_ptr<Reporter> Config::makeReporter(const std::string& serviceName,
                                               logging::Logger& logger,
                                               metrics::Metrics& metrics) const
{
    std::unique_ptr<utils::Transport> transporter =
        _endpoint.empty()
            ? (std::unique_ptr<utils::Transport>(new utils::UDPTransporter(
                  net::IPAddress::v4(_localAgentHostPort), 0)))
            : (std::unique_ptr<utils::Transport>(
                  new utils::HTTPTransporter(net::URI::parse(_endpoint), 0)));

    std::unique_ptr<ThriftSender> sender(new ThriftSender(
        std::forward<std::unique_ptr<utils::Transport>>(transporter)));
    std::unique_ptr<RemoteReporter> remoteReporter(new RemoteReporter(
        _bufferFlushInterval, _queueSize, std::move(sender), logger, metrics));
    if (_logSpans) {
        logger.info("Initializing logging reporter");
        return std::unique_ptr<CompositeReporter>(new CompositeReporter(
            { std::shared_ptr<RemoteReporter>(std::move(remoteReporter)),
              std::make_shared<LoggingReporter>(logger) }));
    }
    return std::unique_ptr<Reporter>(std::move(remoteReporter));
}

void Config::fromEnv()
{
    const auto agentHost =
        utils::EnvVariable::getStringVariable(kJAEGER_AGENT_HOST_ENV_PROP);
    if (!agentHost.empty()) {
        auto agentHostPort = net::IPAddress::parse(_localAgentHostPort);
        std::ostringstream oss;
        oss << agentHost << ":" << agentHostPort.second;
        _localAgentHostPort = oss.str();
    }
    const auto agentPort =
        utils::EnvVariable::getStringVariable(kJAEGER_AGENT_PORT_ENV_PROP);
    if (!agentPort.empty()) {
        std::istringstream iss(agentPort);
        int port = 0;
        if (iss >> port) {
            auto agentHostPort = net::IPAddress::parse(_localAgentHostPort);
            std::ostringstream oss;
            oss << agentHostPort.first << ":" << port;
            _localAgentHostPort = oss.str();
        }
    }

    const auto endpoint =
        utils::EnvVariable::getStringVariable(kJAEGER_ENDPOINT_ENV_PROP);
    if (!endpoint.empty()) {
        _endpoint = endpoint;
    }

    const auto logSpan = utils::EnvVariable::getBoolVariable(
        kJAEGER_REPORTER_LOG_SPANS_ENV_PROP);
    if (logSpan.first) {
        _logSpans = logSpan.second;
    }

    const auto flushInterval = utils::EnvVariable::getIntVariable(
        kJAEGER_REPORTER_FLUSH_INTERVAL_ENV_PROP);
    if (!flushInterval.first) {
        if (flushInterval.second > 0) {
            _bufferFlushInterval =
                std::chrono::milliseconds(flushInterval.second);
        }
    }

    const auto maxQueueSize = utils::EnvVariable::getIntVariable(
        kJAEGER_REPORTER_MAX_QUEUE_SIZE_ENV_PROP);
    if (!maxQueueSize.first) {
        if (maxQueueSize.second > 0) {
            _queueSize = maxQueueSize.second;
        }
    }
}

}  // namespace reporters
}  // namespace jaegertracing
