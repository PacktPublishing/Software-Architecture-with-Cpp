/*
 * Copyright (c) 2019, The Jaeger Authors
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

#include <gtest/gtest.h>

#include <string>

#include <yaml-cpp/yaml.h>

#include "jaegertracing/reporters/Config.h"

namespace jaegertracing {
namespace reporters {

TEST(Config, testLoadConfig)
{
    std::string yamlConfig =
        "disabled: false\n"
        "reporter:\n"
        "    logSpans: true\n"
        "    bufferFlushInterval: 88\n"
        "    localAgentHostPort: ahost:22\n"
        "    endpoint: http://somehost:33/api/traces\n"
        "sampler:\n"
        "  type: const\n"
        "  param: 1";

    auto configYAML = YAML::Load(yamlConfig);
    auto reporterYAML = configYAML["reporter"];
    auto config = jaegertracing::reporters::Config::parse(reporterYAML);

    ASSERT_EQ(true, config.logSpans());
    ASSERT_EQ(std::chrono::seconds(88), config.bufferFlushInterval());
    ASSERT_EQ(std::string("ahost:22"), config.localAgentHostPort());
    ASSERT_EQ(std::string("http://somehost:33/api/traces"), config.endpoint());
}

}  // namespace reporters
}  // namespace jaegertracing
