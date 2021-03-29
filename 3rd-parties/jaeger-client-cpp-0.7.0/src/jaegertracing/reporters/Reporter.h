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

#ifndef JAEGERTRACING_REPORTERS_REPORTER_H
#define JAEGERTRACING_REPORTERS_REPORTER_H

#include "jaegertracing/Compilers.h"

namespace jaegertracing {

class Span;

namespace reporters {

class Reporter {
  public:
    virtual ~Reporter() = default;

    virtual void report(const Span& span) noexcept = 0;

    virtual void close() noexcept = 0;
};

}  // namespace reporters
}  // namespace jaegertracing

#endif  // JAEGERTRACING_REPORTERS_REPORTER_H
