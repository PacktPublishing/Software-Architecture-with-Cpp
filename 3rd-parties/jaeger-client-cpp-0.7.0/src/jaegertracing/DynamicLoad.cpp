/*
 * Copyright (c) 2018 Uber Technologies, Inc.
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
#include <cassert>
#include <cstring>
#include <system_error>

#include <opentracing/dynamic_load.h>

#include "jaegertracing/Tracer.h"
#include "jaegertracing/TracerFactory.h"

static int makeTracerFactory(const char* opentracingVersion,
                             const char* opentracingABIVersion,
                             const void** errorCategory,
                             void* errorMessage,
                             void** tracerFactory)
{
    assert(errorCategory != nullptr);
    assert(tracerFactory != nullptr);
#ifndef JAEGERTRACING_WITH_YAML_CPP
    *errorCategory =
        static_cast<const void*>(&opentracing::dynamic_load_error_category());
    return opentracing::dynamic_load_not_supported_error.value();
#endif
    if (std::strcmp(opentracingABIVersion, OPENTRACING_ABI_VERSION) != 0) {
        *errorCategory = static_cast<const void*>(
            &opentracing::dynamic_load_error_category());
        return opentracing::incompatible_library_versions_error.value();
    }

    const auto jaegerTracerFactory = new (std::nothrow) jaegertracing::TracerFactory(true);
    *tracerFactory = jaegerTracerFactory;
    if (*tracerFactory == nullptr) {
        *errorCategory = static_cast<const void*>(&std::generic_category());
        return static_cast<int>(std::errc::not_enough_memory);
    }

    return 0;
}

OPENTRACING_DECLARE_IMPL_FACTORY(makeTracerFactory)
