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

#include "jaegertracing/net/http/Method.h"
#include <gtest/gtest.h>

namespace jaegertracing {
namespace net {
namespace http {

TEST(Method, testParse)
{
    ASSERT_EQ(Method::OPTIONS, parseMethod("OPTIONS"));
    ASSERT_EQ(Method::GET, parseMethod("GET"));
    ASSERT_EQ(Method::HEAD, parseMethod("HEAD"));
    ASSERT_EQ(Method::POST, parseMethod("POST"));
    ASSERT_EQ(Method::PUT, parseMethod("PUT"));
    ASSERT_EQ(Method::DELETE, parseMethod("DELETE"));
    ASSERT_EQ(Method::TRACE, parseMethod("TRACE"));
    ASSERT_EQ(Method::CONNECT, parseMethod("CONNECT"));
    ASSERT_EQ(Method::EXTENSION, parseMethod("post"));
}

}  // namespace http
}  // namespace net
}  // namespace jaegertracing
