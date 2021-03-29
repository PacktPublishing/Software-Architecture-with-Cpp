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

#ifndef JAEGERTRACING_REPORTERS_INMEMORYREPORTER_H
#define JAEGERTRACING_REPORTERS_INMEMORYREPORTER_H

#include <mutex>
#include <vector>

#include "jaegertracing/Span.h"
#include "jaegertracing/reporters/Reporter.h"

namespace jaegertracing {
namespace reporters {

class InMemoryReporter : public Reporter {
  public:
    InMemoryReporter()
        : _spans()
        , _mutex()
    {
        constexpr auto kInitialCapacity = 10;
        _spans.reserve(kInitialCapacity);
    }

    void report(const Span& span) noexcept override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _spans.push_back(span);
    }

    void close() noexcept override {}

    int spansSubmitted() const noexcept
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _spans.size();
    }

    std::vector<Span> spans() const noexcept
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _spans;
    }

    void reset() noexcept
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _spans.clear();
    }

  private:
    std::vector<Span> _spans;
    mutable std::mutex _mutex;
};

}  // namespace reporters
}  // namespace jaegertracing

#endif  // JAEGERTRACING_REPORTERS_INMEMORYREPORTER_H
