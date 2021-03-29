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

#ifndef JAEGERTRACING_REPORTERS_COMPOSITEREPORTER_H
#define JAEGERTRACING_REPORTERS_COMPOSITEREPORTER_H

#include "jaegertracing/reporters/Reporter.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace jaegertracing {
class Span;
}  // namespace jaegertracing

namespace jaegertracing {
namespace reporters {

class CompositeReporter : public Reporter {
  public:
    using ReporterPtr = std::shared_ptr<Reporter>;

    explicit CompositeReporter(const std::vector<ReporterPtr>& reporters)
        : _reporters(reporters)
    {
    }

    ~CompositeReporter() { close(); }

    void report(const Span& span) noexcept override
    {
        std::for_each(
            std::begin(_reporters),
            std::end(_reporters),
            [&span](const ReporterPtr& reporter) { reporter->report(span); });
    }

    void close() noexcept override
    {
        std::for_each(std::begin(_reporters),
                      std::end(_reporters),
                      [](const ReporterPtr& reporter) { reporter->close(); });
    }

  private:
    std::vector<ReporterPtr> _reporters;
};

}  // namespace reporters
}  // namespace jaegertracing

#endif  // JAEGERTRACING_REPORTERS_COMPOSITEREPORTER_H
