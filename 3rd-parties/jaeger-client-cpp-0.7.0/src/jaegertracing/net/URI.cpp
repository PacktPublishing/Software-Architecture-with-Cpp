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

#include "jaegertracing/net/URI.h"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <regex>
#include <cctype>

namespace jaegertracing {
namespace net {
namespace {

bool isHex(char ch)
{
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') ||
           (ch >= 'a' && ch <= 'f');
}

int decodeHex(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    assert(ch >= 'a' && ch <= 'f');
    return ch - 'a' + 10;
}

bool isUnreserved(char ch)
{
    if (std::isalpha(ch) || std::isdigit(ch)) {
        return true;
    }

    switch (ch) {
    case '-':
    case '.':
    case '_':
    case '~':
        return true;
    default:
        return false;
    }
}

}  // anonymous namespace

URI URI::parse(const std::string& uriStr)
{
    // See https://tools.ietf.org/html/rfc3986 for explanation.
    URI uri;
    std::regex uriRegex(
        "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?",
        std::regex::extended);
    std::smatch match;
    std::regex_match(uriStr, match, uriRegex);

    constexpr auto kSchemeIndex = 2;
    constexpr auto kAuthorityIndex = 4;
    constexpr auto kPathIndex = 5;
    constexpr auto kQueryIndex = 7;

    assert(match.size() >= kQueryIndex);

    uri._scheme = match[kSchemeIndex].str();

    const auto authority = match[kAuthorityIndex].str();
    const auto colonPos = authority.find(':');
    if (colonPos == std::string::npos) {
        uri._host = authority;
    }
    else {
        uri._host = authority.substr(0, colonPos);
        const auto portStr = authority.substr(colonPos + 1);
        std::istringstream iss(portStr);
        iss >> uri._port;
    }

    uri._path = match[kPathIndex].str();
    uri._query = match[kQueryIndex].str();

    return uri;
}

std::string URI::queryEscape(const std::string& input)
{
    std::ostringstream oss;
    for (auto&& ch : input) {
        if (isUnreserved(ch)) {
            oss << ch;
        }
        else {
            oss << '%' << std::uppercase << std::hex << static_cast<int>(ch);
        }
    }
    return oss.str();
}

std::string URI::queryUnescape(const std::string& input)
{
    enum class State { kDefault, kPercent, kFirstHex };

    std::ostringstream oss;
    auto hex = 0;
    auto state = State::kDefault;
    for (auto&& ch : input) {
        switch (state) {
        case State::kDefault: {
            if (ch == '%') {
                state = State::kPercent;
            }
            else {
                oss.put(ch);
            }
        } break;
        case State::kPercent: {
            if (isHex(ch)) {
                state = State::kFirstHex;
                hex = (decodeHex(ch) & 0xff);
            }
            else {
                state = State::kDefault;
                oss.put('%');
                oss.put(ch);
            }
        } break;
        default: {
            assert(state == State::kFirstHex);
            if (isHex(ch)) {
                hex <<= 4;
                hex |= (decodeHex(ch) & 0xff);
                oss.put(static_cast<char>(hex));
            }
            else {
                oss.put('%');
                oss << std::hex << hex;
                oss.put(ch);
            }
            state = State::kDefault;
            hex = 0;
        } break;
        }
    }
    return oss.str();
}

URI::QueryValueMap URI::parseQueryValues() const
{
    enum class State { kKey, kValue };

    if (_query.empty()) {
        return QueryValueMap();
    }

    QueryValueMap values;
    auto state = State::kKey;
    std::string keyBuffer;
    std::string valueBuffer;
    for (auto&& ch : _query) {
        switch (state) {
        case State::kKey: {
            switch (ch) {
            case '=': {
                valueBuffer.clear();
                state = State::kValue;
            } break;
            case '&': {
                values.insert(std::make_pair(queryUnescape(keyBuffer), ""));
                keyBuffer.clear();
                state = State::kKey;
            } break;
            default: {
                keyBuffer.push_back(ch);
            }
            }
        } break;
        default: {
            assert(state == State::kValue);
            if (ch == '&') {
                values.insert(std::make_pair(queryUnescape(keyBuffer),
                                             queryUnescape(valueBuffer)));
                keyBuffer.clear();
                state = State::kKey;
            }
            else {
                valueBuffer.push_back(ch);
            }
        } break;
        }
    }

    if (!keyBuffer.empty()) {
        if (state == State::kKey) {
            values.insert(std::make_pair(queryUnescape(keyBuffer), ""));
        }
        else if (state == State::kValue) {
            values.insert(std::make_pair(queryUnescape(keyBuffer),
                                         queryUnescape(valueBuffer)));
        }
    }
    return values;
}

}  // namespace net
}  // namespace jaegertracing
