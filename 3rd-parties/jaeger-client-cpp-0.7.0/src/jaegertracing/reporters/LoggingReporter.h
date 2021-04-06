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

#ifndef JAEGERTRACING_REPORTERS_LOGGINGREPORTER_H
#define JAEGERTRACING_REPORTERS_LOGGINGREPORTER_H

#include "jaegertracing/Logging.h"
#include "jaegertracing/reporters/Reporter.h"

namespace jaegertracing {
namespace reporters {

class LoggingReporter : public Reporter {
  public:
    explicit LoggingReporter(logging::Logger& logger)
        : _logger(logger)
    {
    }

    void report(const Span& span) noexcept override;

    void close() noexcept override {}

  private:
    logging::Logger& _logger;
};

}  // namespace reporters
}  // namespace jaegertracing

#endif  // JAEGERTRACING_REPORTERS_LOGGINGREPORTER_H
