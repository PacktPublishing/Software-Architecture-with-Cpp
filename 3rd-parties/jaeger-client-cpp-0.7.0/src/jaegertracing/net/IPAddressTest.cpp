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

#include "jaegertracing/net/IPAddress.h"
#include <gtest/gtest.h>
#include <stdexcept>

namespace jaegertracing {
namespace net {

TEST(IPAddress, testParseFail)
{
    ASSERT_THROW(IPAddress::v4("", 0), std::runtime_error);
}

TEST(IPAddress, testAuthority)
{
    ASSERT_EQ("127.0.0.1", IPAddress::v4("127.0.0.1", 0).authority());
    ASSERT_EQ("127.0.0.1:1234", IPAddress::v4("127.0.0.1", 1234).authority());
}

TEST(IPAddress, testIPv6)
{
    ASSERT_EQ("2001:db8:ac10:fe01::",
              IPAddress::v6("2001:db8:ac10:fe01::", 0).authority());
}

TEST(IPAddress, testResolveAddress)
{
    ASSERT_NO_THROW(resolveAddress("localhost", 80, AF_INET, SOCK_STREAM));
#ifdef WIN32
    ASSERT_THROW(resolveAddress("123456", 80, AF_INET, SOCK_STREAM),
                 std::runtime_error);
#else
    ASSERT_NO_THROW(resolveAddress("123456", 80, AF_INET, SOCK_STREAM));
#endif
    ASSERT_THROW(resolveAddress("localhost", 80, -1), std::runtime_error);
}

}  // namespace net
}  // namespace jaegertracing
