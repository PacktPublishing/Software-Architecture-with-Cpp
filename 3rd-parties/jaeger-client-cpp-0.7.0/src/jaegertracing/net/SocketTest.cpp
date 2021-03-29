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
#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/URI.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <system_error>

namespace jaegertracing {
namespace net {

TEST(Socket, testFailOpen)
{
    Socket socket;
    ASSERT_THROW(socket.open(-5, -10), std::system_error);
}

// On Windows, the bind does not throw
 #ifndef WIN32
TEST(Socket, testFailBind)
{
    Socket socket;
    socket.open(AF_INET, SOCK_STREAM);
    ASSERT_THROW(socket.bind(IPAddress::v4("127.0.0.1", 1)), std::system_error);
}
#endif

TEST(Socket, testFailConnect)
{
    Socket socket;
    socket.open(AF_INET, SOCK_STREAM);
    ASSERT_THROW(socket.connect(IPAddress::v4("127.0.0.1", 12345)),
                 std::runtime_error);
}

TEST(Socket, testFailConnectURI)
{
    Socket socket;
    socket.open(AF_INET, SOCK_STREAM);
    const auto uri = URI::parse("http://127.0.0.1:12345");
    ASSERT_THROW(socket.connect(uri), std::runtime_error);
}

TEST(Socket, testFailListen)
{
    Socket socket;
    socket.open(AF_INET, SOCK_DGRAM);
    ASSERT_THROW(socket.listen(), std::system_error);
}

TEST(Socket, testFailAccept)
{
    Socket socket;
    socket.open(AF_INET, SOCK_DGRAM);
    ASSERT_THROW(socket.accept(), std::system_error);
}

}  // namespace net
}  // namespace jaegertracing
