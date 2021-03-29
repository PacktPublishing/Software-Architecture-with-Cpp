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
#include "jaegertracing/thrift-gen/jaeger_types.h"
#include "jaegertracing/thrift-gen/zipkincore_types.h"
#include "jaegertracing/utils/UDPTransporter.h"
#include <future>
#include <gtest/gtest.h>
#include <stdexcept>
#include <thread>
#include <vector>

namespace jaegertracing {
namespace utils {

TEST(UDPSender, testZipkinMessage)
{
    net::IPAddress serverAddr;
    std::promise<void> started;
    std::thread serverThread([&serverAddr, &started]() {
        net::Socket socket;
        socket.open(AF_INET, SOCK_DGRAM);
        socket.bind(net::IPAddress::v4("127.0.0.1", 0));
        ::sockaddr_storage addrStorage;
        ::socklen_t addrLen = sizeof(addrStorage);
        const auto returnCode =
            ::getsockname(socket.handle(),
                          reinterpret_cast<::sockaddr*>(&addrStorage),
                          &addrLen);
        ASSERT_EQ(0, returnCode);
        serverAddr = net::IPAddress(addrStorage, addrLen);
        started.set_value();
    });

    started.get_future().wait();
    UDPTransporter udpClient(serverAddr, 0);
    using ZipkinBatch = std::vector<twitter::zipkin::thrift::Span>;
    ASSERT_THROW(udpClient.emitZipkinBatch(ZipkinBatch()), std::logic_error);
    serverThread.join();
}

TEST(UDPSender, testBigMessage)
{
    net::IPAddress serverAddr;
    std::promise<void> started;
    std::thread serverThread([&serverAddr, &started]() {
        net::Socket socket;
        socket.open(AF_INET, SOCK_DGRAM);
        socket.bind(net::IPAddress::v4("127.0.0.1", 0));
        ::sockaddr_storage addrStorage;
        ::socklen_t addrLen = sizeof(addrStorage);
        const auto returnCode =
            ::getsockname(socket.handle(),
                          reinterpret_cast<::sockaddr*>(&addrStorage),
                          &addrLen);
        ASSERT_EQ(0, returnCode);
        serverAddr = net::IPAddress(addrStorage, addrLen);
        started.set_value();
    });

    started.get_future().wait();
    UDPTransporter udpClient(serverAddr, 1);
    ASSERT_THROW(udpClient.emitBatch(thrift::Batch()), std::logic_error);
    serverThread.join();
}

}  // namespace utils
}  // namespace jaegertracing
