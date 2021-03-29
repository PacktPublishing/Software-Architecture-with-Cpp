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
#include "jaegertracing/testutils/TUDPTransport.h"
#include <array>
#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <thread>

#ifdef _MSC_VER
#pragma warning(disable : 4267)
#endif

namespace jaegertracing {
namespace testutils {
namespace {

constexpr auto kBufferSize = 256;

}  // anonymous namespace

TEST(TUDPTransport, testUDPTransport)
{
    const std::string message("test");

    TUDPTransport server(net::IPAddress::v4("127.0.0.1", 0));
    server.open();  // Not necessary. Just making sure this is called.
    ASSERT_TRUE(server.isOpen());

    const auto serverAddr = server.addr();
    std::thread clientThread([serverAddr, message]() {
        net::Socket connUDP;
        connUDP.open(AF_INET, SOCK_DGRAM);
        const auto numWritten =
            ::sendto(connUDP.handle(),
                     message.c_str(),
                     message.size(),
                     0,
                     reinterpret_cast<const ::sockaddr*>(&serverAddr.addr()),
                     serverAddr.addrLen());
        ASSERT_EQ(numWritten, message.size());

        std::array<char, kBufferSize> buffer;
        const auto numRead = ::recvfrom(
            connUDP.handle(), &buffer[0], buffer.size(), 0, nullptr, 0);
        const std::string received(&buffer[0], &buffer[numRead]);
        ASSERT_EQ(message.size(), numRead);
        ASSERT_EQ(message, received);

        connUDP.close();
    });

    std::array<uint8_t, kBufferSize> buffer;
    const auto numRead = server.readAll(&buffer[0], message.size());
    ASSERT_LT(0, numRead);
    server.write(&buffer[0], numRead);

    clientThread.join();
    server.close();
}

}  // namespace testutils
}  // namespace jaegertracing
