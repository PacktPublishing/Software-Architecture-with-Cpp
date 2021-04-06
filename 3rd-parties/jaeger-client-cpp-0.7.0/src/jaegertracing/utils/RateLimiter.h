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

#ifndef JAEGERTRACING_UTILS_RATELIMITER_H
#define JAEGERTRACING_UTILS_RATELIMITER_H

#include <chrono>
#include <mutex>

namespace jaegertracing {
namespace utils {

template <typename ClockType = std::chrono::steady_clock>
class RateLimiter {
  public:
    using Clock = ClockType;

    RateLimiter(double creditsPerSecond, double maxBalance)
        : _creditsPerSecond(creditsPerSecond)
        , _maxBalance(maxBalance)
        , _balance(_maxBalance)
        , _lastTick(Clock::now())
    {
    }

    bool checkCredit(double itemCost)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const auto currentTime = Clock::now();
        const auto elapsedTime =
            std::chrono::duration<double>(currentTime - _lastTick);
        _lastTick = currentTime;

        _balance += elapsedTime.count() * _creditsPerSecond;
        if (_balance > _maxBalance) {
            _balance = _maxBalance;
        }

        if (_balance >= itemCost) {
            _balance -= itemCost;
            return true;
        }

        return false;
    }

  private:
    double _creditsPerSecond;
    double _maxBalance;
    double _balance;
    typename Clock::time_point _lastTick;
    std::mutex _mutex;
};

}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_UTILS_RATELIMITER_H
