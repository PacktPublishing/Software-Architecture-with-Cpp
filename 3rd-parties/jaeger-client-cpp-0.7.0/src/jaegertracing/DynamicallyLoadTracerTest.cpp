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
#include <iostream>
#include <string>

#include <opentracing/dynamic_load.h>

#include "jaegertracing/Constants.h"

// Verify that Jaeger's shared library can be dynamically loaded and used
// as a plugin.
int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "DynamicLoadTest <tracer-library>\n";
        return -1;
    }
    const char* library = argv[1];
    std::string errorMessage;
    auto tracerFactoryMaybe =
        opentracing::DynamicallyLoadTracingLibrary(library, errorMessage);
#ifdef JAEGERTRACING_WITH_YAML_CPP
    if (!errorMessage.empty()) {
        std::cerr << "Failed to load tracing tracer: " << errorMessage << "\n";
        return -1;
    }
    if (!tracerFactoryMaybe) {
        std::cerr << "Failed to load tracing library: "
                  << tracerFactoryMaybe.error().message() << "\n";
        return -1;
    }
#else
    if (tracerFactoryMaybe) {
        std::cerr << "Dynamically loading a tracing library should fail "
                     "without YAML support\n";
        return -1;
    }
#endif  // JAEGERTRACING_WITH_YAML_CPP
    return 0;
}
