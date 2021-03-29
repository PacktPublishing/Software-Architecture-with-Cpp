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

#ifndef JAEGERTRACING_METRICS_GAUGE_H
#define JAEGERTRACING_METRICS_GAUGE_H

#include "jaegertracing/Compilers.h"

#include <stdint.h>

namespace jaegertracing {
namespace metrics {

class Gauge {
  public:
    virtual ~Gauge() = default;

    virtual void update(int64_t amount) = 0;
};

}  // namespace metrics
}  // namespace jaegertracing

#endif  // JAEGERTRACING_METRICS_GAUGE_H
