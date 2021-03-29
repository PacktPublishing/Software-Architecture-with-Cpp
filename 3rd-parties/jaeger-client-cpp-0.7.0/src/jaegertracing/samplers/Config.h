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

#ifndef JAEGERTRACING_SAMPLERS_CONFIG_H
#define JAEGERTRACING_SAMPLERS_CONFIG_H

#include <cctype>
#include <chrono>
#include <memory>
#include <sstream>
#include <string>

#include "jaegertracing/Constants.h"
#include "jaegertracing/Logging.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/samplers/ConstSampler.h"
#include "jaegertracing/samplers/ProbabilisticSampler.h"
#include "jaegertracing/samplers/RateLimitingSampler.h"
#include "jaegertracing/samplers/RemotelyControlledSampler.h"
#include "jaegertracing/samplers/Sampler.h"
#include "jaegertracing/utils/YAML.h"

namespace jaegertracing {
namespace samplers {

class Config {
  public:
    using Clock = std::chrono::steady_clock;

    static constexpr auto kDefaultSamplingProbability =
        static_cast<double>(0.001);
    static constexpr auto kDefaultSamplingServerURL = "http://127.0.0.1:5778/sampling";
    static constexpr auto kDefaultMaxOperations = 2000;

    static constexpr auto kJAEGER_SAMPLER_TYPE_ENV_PROP = "JAEGER_SAMPLER_TYPE";
    static constexpr auto kJAEGER_SAMPLER_PARAM_ENV_PROP = "JAEGER_SAMPLER_PARAM";
    static constexpr auto kJAEGER_SAMPLING_ENDPOINT_ENV_PROP = "JAEGER_SAMPLING_ENDPOINT";

    static Clock::duration defaultSamplingRefreshInterval()
    {
        return std::chrono::minutes(1);
    }

#ifdef JAEGERTRACING_WITH_YAML_CPP

    static Config parse(const YAML::Node& configYAML)
    {
        if (!configYAML.IsDefined() || !configYAML.IsMap()) {
            return Config();
        }

        const auto type =
            utils::yaml::findOrDefault<std::string>(configYAML, "type", "");
        const auto param =
            utils::yaml::findOrDefault<double>(configYAML, "param", -1);
        const auto samplingServerURL = utils::yaml::findOrDefault<std::string>(
            configYAML, "samplingServerURL", "");
        const auto maxOperations =
            utils::yaml::findOrDefault<int>(configYAML, "maxOperations", 0);
        const auto samplingRefreshInterval =
            std::chrono::seconds(utils::yaml::findOrDefault<int>(
                configYAML, "samplingRefreshInterval", 0));
        return Config(type,
                      param,
                      samplingServerURL,
                      maxOperations,
                      samplingRefreshInterval);
    }

#endif  // JAEGERTRACING_WITH_YAML_CPP

    explicit Config(
        const std::string& type = kSamplerTypeRemote,
        double param = kDefaultSamplingProbability,
        const std::string& samplingServerURL = kDefaultSamplingServerURL,
        int maxOperations = kDefaultMaxOperations,
        const Clock::duration& samplingRefreshInterval =
            defaultSamplingRefreshInterval())
        : _type(type.empty() ? kSamplerTypeRemote : type)
        , _param(param == -1 ? kDefaultSamplingProbability : param)
        , _samplingServerURL(samplingServerURL.empty()
                                 ? kDefaultSamplingServerURL
                                 : samplingServerURL)
        , _maxOperations(maxOperations == 0 ? kDefaultMaxOperations
                                            : maxOperations)
        , _samplingRefreshInterval(samplingRefreshInterval.count() > 0
                                       ? samplingRefreshInterval
                                       : defaultSamplingRefreshInterval())
    {
    }

    std::unique_ptr<Sampler> makeSampler(const std::string& serviceName,
                                         logging::Logger& logger,
                                         metrics::Metrics& metrics) const
    {
        std::string samplerType;
        samplerType.reserve(_type.size());
        std::transform(std::begin(_type),
                       std::end(_type),
                       std::back_inserter(samplerType),
                       [](const char ch) { return std::tolower(ch); });
        if (samplerType == kSamplerTypeConst) {
            return std::unique_ptr<ConstSampler>(new ConstSampler(_param != 0));
        }

        if (samplerType == kSamplerTypeProbabilistic) {
            if (_param >= 0 && _param <= 1) {
                return std::unique_ptr<ProbabilisticSampler>(
                    new ProbabilisticSampler(_param));
            }
            else {
                std::ostringstream oss;
                oss << "Invalid parameter for probabilistic sampler: " << _param
                    << ", expecting value between 0 and 1";
                logger.error(oss.str());
                throw std::invalid_argument("Probabilistic sampling rate should be in the interval [0, 1].");
                return std::unique_ptr<Sampler>();
            }
        }

        if (samplerType == kSamplerTypeRateLimiting) {
            return std::unique_ptr<RateLimitingSampler>(
                new RateLimitingSampler(_param));
        }

        if (samplerType == kSamplerTypeRemote || samplerType.empty()) {
            auto config = *this;
            config._type = kSamplerTypeProbabilistic;
            std::shared_ptr<Sampler> initSampler(
                config.makeSampler(serviceName, logger, metrics));
            if (!initSampler) {
                return std::unique_ptr<Sampler>();
            }

            return std::unique_ptr<RemotelyControlledSampler>(
                new RemotelyControlledSampler(serviceName,
                                              _samplingServerURL,
                                              initSampler,
                                              _maxOperations,
                                              _samplingRefreshInterval,
                                              logger,
                                              metrics));
        }

        std::ostringstream oss;
        oss << "Unknown sampler type " << _type;
        logger.error(oss.str());
        return std::unique_ptr<Sampler>();
    }

    const std::string& type() const { return _type; }

    double param() const { return _param; }

    const std::string& samplingServerURL() const { return _samplingServerURL; }

    int maxOperations() const { return _maxOperations; }

    const Clock::duration& samplingRefreshInterval() const
    {
        return _samplingRefreshInterval;
    }

  void fromEnv();

  private:
    std::string _type;
    double _param;
    std::string _samplingServerURL;
    int _maxOperations;
    Clock::duration _samplingRefreshInterval;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_CONFIG_H
