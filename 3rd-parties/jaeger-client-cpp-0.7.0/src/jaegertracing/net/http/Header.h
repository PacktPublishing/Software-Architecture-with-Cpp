/*
 * Copyright (c) 2017-2018 Uber Technologies, Inc.
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

#ifndef JAEGERTRACING_NET_HTTP_HEADER_H
#define JAEGERTRACING_NET_HTTP_HEADER_H

#include <cassert>
#include <regex>
#include <string>

#include "jaegertracing/net/http/Error.h"

namespace jaegertracing {
namespace net {
namespace http {

class Header {
  public:
    Header() = default;

    Header(const std::string& key, const std::string& value)
        : _key(key)
        , _value(value)
    {
    }

    const std::string& key() const { return _key; }

    const std::string& value() const { return _value; }

  private:
    std::string _key;
    std::string _value;
};

inline std::istream& readLineCRLF(std::istream& in, std::string& line)
{
    line.clear();
    auto ch = '\0';
    auto sawCR = false;
    while (in.get(ch)) {
        if (sawCR) {
            if (ch == '\n') {
                break;
            }
            else {
                line.push_back('\r');
                line.push_back(ch);
                sawCR = false;
            }
        }
        else {
            if (ch == '\r') {
                sawCR = true;
            }
            else {
                line.push_back(ch);
            }
        }
    }

    return in;
}

inline void readHeaders(std::istream& in, std::vector<Header>& headers)
{
    const std::regex headerPattern("([^:]+): (.+)$");
    std::string line;
    std::smatch match;
    while (readLineCRLF(in, line)) {
        if (line.empty()) {
            break;
        }
        if (!std::regex_match(line, match, headerPattern) || match.size() < 3) {
            throw ParseError::make("header", line);
        }
        headers.emplace_back(Header(match[1], match[2]));
    }
}

}  // namespace http
}  // namespace net
}  // namespace jaegertracing

#endif  // JAEGERTRACING_NET_HTTP_HEADER_H
