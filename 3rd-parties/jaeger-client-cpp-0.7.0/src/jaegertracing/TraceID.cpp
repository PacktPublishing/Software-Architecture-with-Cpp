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

#include "jaegertracing/TraceID.h"
#include "jaegertracing/utils/HexParsing.h"
#include <iterator>
#include <stddef.h>
#include <string>

namespace jaegertracing {

TraceID TraceID::fromStream(std::istream& in)
{
    TraceID traceID;
    constexpr auto kMaxChars = static_cast<size_t>(32);
    auto buffer = utils::HexParsing::readSegment(in, kMaxChars, ':');

    if (buffer.empty()) {
        return TraceID();
    }

    if (buffer.size() < kMaxChars / 2) {
        traceID._low = utils::HexParsing::decodeHex<uint64_t>(buffer);
    }
    else {
        auto beginLowStr = std::end(buffer) - kMaxChars / 2;
        const std::string highStr(std::begin(buffer), beginLowStr);
        traceID._high = utils::HexParsing::decodeHex<uint64_t>(highStr);
        const std::string lowStr(beginLowStr, std::end(buffer));
        traceID._low = utils::HexParsing::decodeHex<uint64_t>(lowStr);
    }

    return traceID;
}

}  // namespace jaegertracing
