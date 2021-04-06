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

#ifndef JAEGERTRACING_BAGGAGE_RESTRICTIONSCONFIG_H
#define JAEGERTRACING_BAGGAGE_RESTRICTIONSCONFIG_H

#include "jaegertracing/Constants.h"
#include "jaegertracing/utils/YAML.h"
#include <chrono>
#include <string>

namespace jaegertracing {
namespace baggage {

class RestrictionsConfig {
  public:
    using Clock = std::chrono::steady_clock;

#ifdef JAEGERTRACING_WITH_YAML_CPP

    static RestrictionsConfig parse(const YAML::Node& configYAML)
    {
        if (!configYAML.IsDefined() || !configYAML.IsMap()) {
            return RestrictionsConfig();
        }

        const auto denyBaggageOnInitializationFailure =
            utils::yaml::findOrDefault<bool>(
                configYAML, "denyBaggageOnInitializationFailure", false);
        const auto hostPort =
            utils::yaml::findOrDefault<std::string>(configYAML, "hostPort", "");
        const auto refreshInterval = std::chrono::seconds(
            utils::yaml::findOrDefault<int>(configYAML, "refreshInterval", 0));
        return RestrictionsConfig(
            denyBaggageOnInitializationFailure, hostPort, refreshInterval);
    }

#endif  // JAEGERTRACING_WITH_YAML_CPP

    explicit RestrictionsConfig(
        bool denyBaggageOnInitializationFailure = false,
        const std::string& hostPort = "",
        const Clock::duration& refreshInterval = Clock::duration())
        : _denyBaggageOnInitializationFailure(
              denyBaggageOnInitializationFailure)
        , _hostPort(hostPort)
        , _refreshInterval(refreshInterval)
    {
    }

    bool denyBaggageOnInitializationFailure() const
    {
        return _denyBaggageOnInitializationFailure;
    }

    const std::string& hostPort() const { return _hostPort; }

    const Clock::duration& refreshInterval() const { return _refreshInterval; }

  private:
    bool _denyBaggageOnInitializationFailure;
    std::string _hostPort;
    Clock::duration _refreshInterval;
};

}  // namespace baggage
}  // namespace jaegertracing

#endif  // JAEGERTRACING_BAGGAGE_RESTRICTIONSCONFIG_H
