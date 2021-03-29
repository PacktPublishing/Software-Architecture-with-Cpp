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

#ifndef JAEGERTRACING_PLATFORM_HOSTNAME_H
#define JAEGERTRACING_PLATFORM_HOSTNAME_H

#include <errno.h>
#include <string>
#include <system_error>

#ifdef WIN32
#include <Winsock2.h>
#else
#include <unistd.h>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4267)
#endif

namespace jaegertracing {
namespace platform {

inline std::string hostname()
{
    constexpr auto kBufferSize = 256;
    std::string buffer(kBufferSize, '\0');
    const auto returnCode = ::gethostname(&buffer[0], buffer.size());
    if (returnCode != 0) {
        throw std::system_error(
            errno, std::system_category(), "Failed to get hostname");
    }
    const auto nullPos = buffer.find('\0');
    return buffer.substr(0, nullPos);
}

}  // namespace platform
}  // namespace jaegertracing

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // JAEGERTRACING_PLATFORM_HOSTNAME_H
