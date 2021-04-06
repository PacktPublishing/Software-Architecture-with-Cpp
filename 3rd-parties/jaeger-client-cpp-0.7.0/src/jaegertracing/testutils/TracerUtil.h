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

#ifndef JAEGERTRACING_TESTUTILS_TRACERUTIL_H
#define JAEGERTRACING_TESTUTILS_TRACERUTIL_H

#include <string>

#include "jaegertracing/Tracer.h"
#include "jaegertracing/propagation/Format.h"
#include "jaegertracing/testutils/MockAgent.h"

namespace jaegertracing {
namespace testutils {
namespace TracerUtil {

struct ResourceHandle {
    ResourceHandle()
        : _mockAgent(testutils::MockAgent::make())
    {
    }

    ~ResourceHandle()
    {
        opentracing::Tracer::InitGlobal(opentracing::MakeNoopTracer());
    }

    std::shared_ptr<MockAgent> _mockAgent;
};

std::shared_ptr<ResourceHandle> installGlobalTracer();
std::shared_ptr<ResourceHandle> installGlobalTracer128Bit();
std::shared_ptr<ResourceHandle> installGlobalTracerW3CPropagation();
std::shared_ptr<opentracing::Tracer> buildTracer(const std::string& endpoint);

}  // namespace TracerUtil
}  // namespace testutils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_TESTUTILS_TRACERUTIL_H
