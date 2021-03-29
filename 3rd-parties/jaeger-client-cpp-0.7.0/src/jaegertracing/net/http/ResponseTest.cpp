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

#include "jaegertracing/Constants.h"
#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/URI.h"
#include "jaegertracing/net/http/Response.h"
#include <array>
#include <future>
#include <gtest/gtest.h>
#include <ostream>
#include <string>
#include <thread>

#ifdef _MSC_VER
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#endif

namespace jaegertracing {
namespace net {
namespace http {

namespace {

#ifdef WIN32
#define READ_ERROR SOCKET_ERROR
#else
#define READ_ERROR -1
#endif

static size_t read(int socketHandle, char* buffer, size_t size)
{
    int returnValue = ::recv(socketHandle, buffer, size, 0);
    return (returnValue == READ_ERROR) ? 0 : returnValue;
}
}

TEST(Response, get)
{
    IPAddress serverAddress;
    std::promise<void> started;

    std::thread clientThread([&serverAddress, &started]() {
        started.get_future().wait();
        get(URI::parse("http://" + serverAddress.authority()));
    });

    Socket socket;
    socket.open(AF_INET, SOCK_STREAM);
    socket.bind(IPAddress::v4("127.0.0.1", 0));
    socket.listen();

    ::sockaddr_storage addrStorage;
    ::socklen_t addrLen = sizeof(addrStorage);
    const auto returnCode = ::getsockname(
        socket.handle(), reinterpret_cast<::sockaddr*>(&addrStorage), &addrLen);
    ASSERT_EQ(0, returnCode);
    serverAddress = IPAddress(addrStorage, addrLen);

    started.set_value();

    auto clientSocket = socket.accept();
    std::array<char, 256> buffer;

    const auto numRead = read(clientSocket.handle(), &buffer[0], buffer.size());

    std::ostringstream oss;
    oss << "GET / HTTP/1.1\r\n"
           "Host: 127.0.0.1:"
        << serverAddress.port() << "\r\n"
        << "User-Agent: jaegertracing/" << kJaegerClientVersion << "\r\n\r\n";
    ASSERT_EQ(oss.str(), std::string(&buffer[0], numRead));

    oss.str("");
    oss.clear();

    std::string response("HTTP/1.1 200 OK\r\n\r\n");
    response.append(buffer.size() * 2, '0');
    const auto numWritten =
        ::send(clientSocket.handle(), response.c_str(), response.size(), 0);
    ASSERT_EQ(response.size(), numWritten);

    clientThread.join();
}

}  // namespace http
}  // namespace net
}  // namespace jaegertracing
