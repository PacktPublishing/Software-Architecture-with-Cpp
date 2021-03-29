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

#ifndef JAEGERTRACING_REPORTERS_REMOTEREPORTER_H
#define JAEGERTRACING_REPORTERS_REMOTEREPORTER_H

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/Logging.h"
#include "jaegertracing/Span.h"
#include "jaegertracing/Sender.h"
#include "jaegertracing/metrics/Metrics.h"
#include "jaegertracing/reporters/Reporter.h"

namespace jaegertracing {
namespace reporters {

class RemoteReporter : public Reporter {
  public:
    using Clock = std::chrono::steady_clock;

    RemoteReporter(const Clock::duration& bufferFlushInterval,
                   int fixedQueueSize,
                   std::unique_ptr<Sender>&& sender,
                   logging::Logger& logger,
                   metrics::Metrics& metrics);

    ~RemoteReporter() { close(); }

    void report(const Span& span) noexcept override;

    void close() noexcept override;

  private:
    void sweepQueue() noexcept;

    void sendSpan(const Span& span) noexcept;

    void flush() noexcept;

    bool bufferFlushIntervalExpired() const
    {
        return (Clock::now() - _lastFlush) >= _bufferFlushInterval;
    }

    Clock::duration _bufferFlushInterval;
    int _fixedQueueSize;
    std::unique_ptr<Sender> _sender;
    logging::Logger& _logger;
    metrics::Metrics& _metrics;
    std::deque<Span> _queue;
    std::atomic<int> _queueLength;
    bool _running;
    Clock::time_point _lastFlush;
    std::condition_variable _cv;
    std::mutex _mutex;
    std::thread _thread;
};

}  // namespace reporters
}  // namespace jaegertracing

#endif  // JAEGERTRACING_REPORTERS_REMOTEREPORTER_H
