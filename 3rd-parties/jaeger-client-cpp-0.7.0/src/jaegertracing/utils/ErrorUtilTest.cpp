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
#include "jaegertracing/Sender.h"
#include "jaegertracing/utils/ErrorUtil.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <iosfwd>
#include <iterator>
#include <stdexcept>
#include <string>
#include <system_error>

namespace jaegertracing {
namespace utils {
namespace {

class TestLogger : public logging::Logger {
  public:
    void info(const std::string&) override {}

    void error(const std::string& message) override { _message = message; }

    const std::string& str() const { return _message; }

  private:
    std::string _message;
};

bool endsWith(const std::string& str, const std::string& suffix)
{
    if (str.size() < suffix.size()) {
        return false;
    }

    return std::equal(
        std::begin(suffix), std::end(suffix), std::end(str) - suffix.size());
}

}  // anonymous namespace

TEST(ErrorUtil, test)
{
    std::ostringstream oss;
    TestLogger logger;
    std::runtime_error stdEx("runtime error");
    std::system_error sysEx(-1, std::generic_category());
    Sender::Exception transportEx("test", 5);
    for (auto i = 0; i < 4; ++i) {
        try {
            switch (i) {
            case 0:
                throw stdEx;
            case 1:
                throw sysEx;
            case 2:
                throw transportEx;
            default:
                ASSERT_EQ(3, i);
                throw 5;
            }
        } catch (...) {
            ErrorUtil::logError(logger, "test");
            switch (i) {
            case 0: {
                ASSERT_EQ("test: runtime error", logger.str());
            } break;
            case 1: {
                ASSERT_TRUE(endsWith(logger.str(), ", code=-1"));
            } break;
            case 2: {
                ASSERT_EQ("test: test, numFailed=5", logger.str());
            } break;
            default: {
                ASSERT_EQ(3, i);
                ASSERT_EQ("test", logger.str());
            } break;
            }
        }
    }
}

}  // namespace utils
}  // namespace jaegertracing
