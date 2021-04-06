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

#include "jaegertracing/utils/RateLimiter.h"
#include <chrono>
#include <gtest/gtest.h>

namespace jaegertracing {
namespace utils {
namespace {

std::chrono::steady_clock::time_point currentTime;

class MockClock {
  public:
    using rep = std::chrono::steady_clock::rep;
    using period = std::chrono::steady_clock::period;
    using duration = std::chrono::steady_clock::duration;
    using time_point = std::chrono::steady_clock::time_point;

    static const bool is_steady() { return false; }

    static time_point now() { return currentTime; }
};

}  // anonymous namespace

TEST(RateLimiter, testRateLimiter)
{
    const auto timestamp = std::chrono::steady_clock::now();
    currentTime = timestamp;
    RateLimiter<MockClock> limiter(2, 2);

    ASSERT_TRUE(limiter.checkCredit(1));
    ASSERT_TRUE(limiter.checkCredit(1));
    ASSERT_FALSE(limiter.checkCredit(1));

    currentTime = timestamp + std::chrono::milliseconds(250);
    ASSERT_FALSE(limiter.checkCredit(1));

    currentTime = timestamp + std::chrono::milliseconds(750);
    ASSERT_TRUE(limiter.checkCredit(1));
    ASSERT_FALSE(limiter.checkCredit(1));

    currentTime = timestamp + std::chrono::seconds(5);
    ASSERT_TRUE(limiter.checkCredit(1));
    ASSERT_TRUE(limiter.checkCredit(1));
    ASSERT_FALSE(limiter.checkCredit(1));
    ASSERT_FALSE(limiter.checkCredit(1));
    ASSERT_FALSE(limiter.checkCredit(1));
}

TEST(RateLimiter, testMaxBalance)
{
    const auto timestamp = std::chrono::steady_clock::now();
    currentTime = timestamp;
    RateLimiter<MockClock> limiter(0.1, 1.0);

    ASSERT_TRUE(limiter.checkCredit(1.0));

    currentTime = timestamp + std::chrono::seconds(20);
    ASSERT_TRUE(limiter.checkCredit(1.0));
    ASSERT_FALSE(limiter.checkCredit(1.0));
}

}  // namespace utils
}  // namespace jaegertracing
