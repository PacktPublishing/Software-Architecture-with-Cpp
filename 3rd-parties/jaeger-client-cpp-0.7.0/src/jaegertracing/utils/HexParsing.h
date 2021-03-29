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

#ifndef JAEGERTRACING_UTILS_HEXPARSING_H
#define JAEGERTRACING_UTILS_HEXPARSING_H

#include <cassert>
#include <iomanip>
#include <iostream>
#include <cctype>

namespace jaegertracing {
namespace utils {
namespace HexParsing {

inline bool isHex(char ch)
{
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') ||
           (ch >= 'a' && ch <= 'f');
}

inline std::string readSegment(std::istream& in, size_t maxChars, char delim)
{
    std::string buffer;
    auto ch = '\0';
    for (auto i = static_cast<size_t>(0); i < maxChars && in.get(ch); ++i) {
        if (!isHex(ch)) {
            if (ch == delim) {
                in.putback(ch);
                break;
            }
            else {
                return "";
            }
        }

        buffer.push_back(ch);
    }
    return buffer;
}

template <typename ResultType>
ResultType decodeHex(const std::string& str)
{
    auto first = std::begin(str);
    auto last = std::end(str);
    ResultType result = 0;
    for (; first != last; ++first) {
        const auto ch = *first;

        // This condition is guaranteed by `readSegment`.
        assert(isHex(ch));

        auto hexDigit = 0;
        if (std::isdigit(ch)) {
            hexDigit = (ch - '0');
        }
        else if (std::isupper(ch)) {
            hexDigit = (ch - 'A') + 10;
        }
        else {
            hexDigit = (ch - 'a') + 10;
        }

        result = (result << 4) | hexDigit;
    }

    return result;
}

}  // namespace HexParsing
}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_UTILS_HEXPARSING_H
