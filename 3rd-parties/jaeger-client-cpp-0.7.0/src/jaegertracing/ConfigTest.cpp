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

#include "jaegertracing/Config.h"
#include "jaegertracing/Constants.h"
#include "jaegertracing/propagation/HeadersConfig.h"
#include "jaegertracing/samplers/Config.h"
#include "jaegertracing/utils/YAML.h"
#include "jaegertracing/testutils/EnvVariable.h"
#include <gtest/gtest.h>

#include <cstdlib>

namespace jaegertracing {

#ifdef JAEGERTRACING_WITH_YAML_CPP

TEST(Config, testParse)
{
    {
        constexpr auto kConfigYAML = R"cfg(
disabled: true
traceid_128bit: true
sampler:
    type: probabilistic
    param: 0.001
reporter:
    queueSize: 100
    bufferFlushInterval: 10
    logSpans: false
    localAgentHostPort: 127.0.0.1:6831
headers:
    jaegerDebugHeader: debug-id
    jaegerBaggageHeader: baggage
    TraceContextHeaderName: trace-id
    traceBaggageHeaderPrefix: "testctx-"
baggage_restrictions:
    denyBaggageOnInitializationFailure: false
    hostPort: 127.0.0.1:5778
    refreshInterval: 60
)cfg";
        const auto config = Config::parse(YAML::Load(kConfigYAML));
        ASSERT_EQ("probabilistic", config.sampler().type());
        ASSERT_EQ("debug-id", config.headers().jaegerDebugHeader());
        ASSERT_EQ("baggage", config.headers().jaegerBaggageHeader());
        ASSERT_EQ("trace-id", config.headers().traceContextHeaderName());
        ASSERT_EQ("testctx-", config.headers().traceBaggageHeaderPrefix());
    }

    {
        Config::parse(YAML::Load(R"cfg(
disabled: false
traceid_128bit: false
sampler: 1
reporter: 2
headers: 3
baggage_restrictions: 4
)cfg"));
    }
}

TEST(Config, testDefaultSamplingProbability)
{
    ASSERT_EQ(samplers::Config::kDefaultSamplingProbability,
              Config().sampler().param());
}

TEST(Config, testDefaultSamplingServerURL)
{
    ASSERT_EQ("http://127.0.0.1:5778/sampling",
              Config().sampler().samplingServerURL());
}

TEST(Config, testZeroSamplingParam)
{
    {
        constexpr auto kConfigYAML = R"cfg(
sampler:
    param: 0
)cfg";
        const auto config = Config::parse(YAML::Load(kConfigYAML));
        ASSERT_EQ(0, config.sampler().param());
    }
}

TEST(Config, testPropagationFormat)
{
    {
        constexpr auto kConfigYAML = R"cfg(
propagation_format: w3c
)cfg";
        const auto config = Config::parse(YAML::Load(kConfigYAML));
        ASSERT_EQ(propagation::Format::W3C, config.propagationFormat());
    }
}

#endif  // JAEGERTRACING_WITH_YAML_CPP

TEST(Config, testFromEnv)
{
    std::vector<Tag> tags;
    tags.emplace_back("hostname", std::string("foo"));
    tags.emplace_back("my.app.version", std::string("1.2.3"));

    Config config(false,
                  false,
                  samplers::Config("probabilistic",
                                   0.7,
                                   "http://host34:57/sampling",
                                   0,
                                   samplers::Config::Clock::duration()),
                  reporters::Config(10,
                                    std::chrono::milliseconds(100),
                                    false,
                                    "host35:77",
                                    "http://host36:56568"),
                  propagation::HeadersConfig(),
                  baggage::RestrictionsConfig(),
                  "test-service",
                  tags);

    config.fromEnv();

    ASSERT_EQ(std::string("http://host36:56568"), config.reporter().endpoint());
    ASSERT_EQ(std::string("host35:77"), config.reporter().localAgentHostPort());

    ASSERT_EQ(10, config.reporter().queueSize());
    ASSERT_EQ(std::chrono::milliseconds(100),
              config.reporter().bufferFlushInterval());
    ASSERT_EQ(false, config.reporter().logSpans());

    ASSERT_EQ(.7, config.sampler().param());
    ASSERT_EQ(std::string("probabilistic"), config.sampler().type());

    testutils::EnvVariable::setEnv("JAEGER_AGENT_HOST", "host33");
    testutils::EnvVariable::setEnv("JAEGER_AGENT_PORT", "45");
    testutils::EnvVariable::setEnv("JAEGER_ENDPOINT", "http://host34:56567");

    testutils::EnvVariable::setEnv("JAEGER_REPORTER_MAX_QUEUE_SIZE", "33");
    testutils::EnvVariable::setEnv("JAEGER_REPORTER_FLUSH_INTERVAL", "45");
    testutils::EnvVariable::setEnv("JAEGER_REPORTER_LOG_SPANS", "true");

    testutils::EnvVariable::setEnv("JAEGER_SAMPLER_TYPE", "remote");
    testutils::EnvVariable::setEnv("JAEGER_SAMPLER_PARAM", "0.33");
    testutils::EnvVariable::setEnv("JAEGER_SAMPLING_ENDPOINT", "http://myagent:1234");

    testutils::EnvVariable::setEnv("JAEGER_SERVICE_NAME", "AService");
    testutils::EnvVariable::setEnv("JAEGER_TAGS", "hostname=foobar,my.app.version=4.5.6");

    testutils::EnvVariable::setEnv("JAEGER_TRACEID_128BIT", "true");

    config.fromEnv();

    ASSERT_EQ(std::string("http://host34:56567"), config.reporter().endpoint());
    ASSERT_EQ(std::string("host33:45"), config.reporter().localAgentHostPort());

    ASSERT_EQ(33, config.reporter().queueSize());
    ASSERT_EQ(std::chrono::milliseconds(45),
              config.reporter().bufferFlushInterval());
    ASSERT_EQ(true, config.reporter().logSpans());

    ASSERT_EQ(std::string("remote"), config.sampler().type());
    ASSERT_EQ(0.33, config.sampler().param());
    ASSERT_EQ(std::string("http://myagent:1234"), config.sampler().samplingServerURL());

    ASSERT_EQ(std::string("AService"), config.serviceName());

    std::vector<Tag> expectedTags;
    expectedTags.emplace_back("hostname", std::string("foo"));
    expectedTags.emplace_back("my.app.version", std::string("1.2.3"));
    expectedTags.emplace_back("hostname", std::string("foobar"));
    expectedTags.emplace_back("my.app.version", std::string("4.5.6"));
    ASSERT_EQ(expectedTags, config.tags());

    ASSERT_EQ(false, config.disabled());

    ASSERT_EQ(true, config.traceId128Bit());

    testutils::EnvVariable::setEnv("JAEGER_DISABLED", "TRue");  // case-insensitive
    testutils::EnvVariable::setEnv("JAEGER_AGENT_PORT", "445");

    config.fromEnv();
    ASSERT_EQ(true, config.disabled());
    ASSERT_EQ(std::string("host33:445"),
              config.reporter().localAgentHostPort());

    testutils::EnvVariable::setEnv("JAEGER_PROPAGATION", "w3c");

    config.fromEnv();
    ASSERT_EQ(propagation::Format::W3C, config.propagationFormat());

    testutils::EnvVariable::setEnv("JAEGER_AGENT_HOST", "");
    testutils::EnvVariable::setEnv("JAEGER_AGENT_PORT", "");
    testutils::EnvVariable::setEnv("JAEGER_ENDPOINT", "");
    testutils::EnvVariable::setEnv("JAEGER_REPORTER_MAX_QUEUE_SIZE", "");
    testutils::EnvVariable::setEnv("JAEGER_REPORTER_FLUSH_INTERVAL", "");
    testutils::EnvVariable::setEnv("JAEGER_REPORTER_LOG_SPANS", "");
    testutils::EnvVariable::setEnv("JAEGER_SAMPLER_PARAM", "");
    testutils::EnvVariable::setEnv("JAEGER_SAMPLER_TYPE", "");
    testutils::EnvVariable::setEnv("JAEGER_SERVICE_NAME", "");
    testutils::EnvVariable::setEnv("JAEGER_TAGS", "");
    testutils::EnvVariable::setEnv("JAEGER_DISABLED", "");
    testutils::EnvVariable::setEnv("JAEGER_TRACE_ID_128BIT", "");
    testutils::EnvVariable::setEnv("JAEGER_PROPAGATION", "");
}

}  // namespace jaegertracing
