/*
 * Copyright (c) 2019 The Jaeger Authors.
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

#ifndef JAEGERTRACING_UTILS_ENV_VARIABLE_H
#define JAEGERTRACING_UTILS_ENV_VARIABLE_H

#include <string>
#include <sstream>
#include <algorithm>

namespace jaegertracing {
namespace utils {
namespace EnvVariable {

inline std::string getStringVariable(const char* envVar)
{
    const auto rawVariable = std::getenv(envVar);
    return std::string(rawVariable ? rawVariable : "");
}

inline std::pair<bool, int> getIntVariable(const char* envVar)
{
    const auto rawVariable = std::getenv(envVar);
    const std::string variable(rawVariable ? rawVariable : "");
    if (!variable.empty()) {
        std::istringstream iss(variable);
        int intVal = 0;
        if (iss >> intVal) {
            return std::make_pair(false, intVal);
        }
    }
    return std::make_pair(false, 0);
}

inline std::pair<bool, bool> getBoolVariable(const char* envVar)
{
    const auto rawVariable = std::getenv(envVar);
    std::string variable(rawVariable ? rawVariable : "");

    if (!variable.empty()) {
        std::transform(
            variable.begin(), variable.end(), variable.begin(), ::tolower);
        return std::make_pair(true, (variable == "true"));
    }
    return std::make_pair(false, false);
}

}  // namespace EnvVariable
}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_UTILS_ENV_VARIABLE_H
