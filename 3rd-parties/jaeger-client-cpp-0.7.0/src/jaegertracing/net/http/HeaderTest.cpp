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

#include "jaegertracing/net/http/Header.h"
#include <gtest/gtest.h>
#include <ostream>
#include <string>
#include <vector>

namespace jaegertracing {
namespace net {
namespace http {

class ParseError;

TEST(Header, readLine)
{
    std::stringstream ss;
    ss << "test\r123\r\n";
    std::string line;
    while (readLineCRLF(ss, line)) {
        ASSERT_EQ("test\r123", line);
    }
}

TEST(Header, readHeaders)
{
    std::stringstream ss;
    ss << "Bad Header\r\n";
    std::vector<Header> headers;
    ASSERT_THROW(readHeaders(ss, headers), ParseError);
}

}  // namespace http
}  // namespace net
}  // namespace jaegertracing
