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

#include "jaegertracing/Logging.h"

#include <iostream>

namespace jaegertracing {
namespace logging {
namespace {

class NullLogger : public Logger {
  public:
    void error(const std::string& /* message */) override {}

    void info(const std::string& /* message */) override {}
};

class ConsoleLogger : public Logger {
  public:
    void error(const std::string& message) override
    {
        std::cerr << "ERROR: " << message << '\n';
    }

    void info(const std::string& message) override
    {
        std::cout << "INFO: " << message << '\n';
    }
};

}  // anonymous namespace

std::unique_ptr<Logger> nullLogger()
{
    return std::unique_ptr<Logger>(new NullLogger());
}

std::unique_ptr<Logger> consoleLogger()
{
    return std::unique_ptr<Logger>(new ConsoleLogger());
}

}  // namespace logging
}  // namespace jaegertracing
