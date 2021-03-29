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
#include "jaegertracing/testutils/TracerUtil.h"
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <opentracing/string_view.h>
#include <opentracing/tracer.h>
#include <stdexcept>
#include <string>
#include <utility>

#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/http/Request.h"
#include "jaegertracing/net/http/Response.h"
#include <future>
#include <thread>

namespace jaegertracing {

namespace utils {

TEST(HTTPTransporter, testSpanReporting)
{
    net::IPAddress serverAddr;
    std::promise<void> started;

    std::string target;
    net::http::Method method;
    std::string contentType;
    std::string acceptType;

    std::thread serverThread(
        [&serverAddr, &started, &target, &method, &contentType, &acceptType]() {
            net::Socket socket;
            socket.open(AF_INET, SOCK_STREAM);
            socket.bind(net::IPAddress::v4("127.0.0.1", 0));
            ::sockaddr_storage addrStorage;
            ::socklen_t addrLen = sizeof(addrStorage);
            const auto returnCode =
                ::getsockname(socket.handle(),
                              reinterpret_cast<::sockaddr*>(&addrStorage),
                              &addrLen);
            ASSERT_EQ(0, returnCode);
            serverAddr = net::IPAddress(addrStorage, addrLen);

            // Listening to the port
            socket.listen();

            // Unblocking the client
            started.set_value();

            // Waiting for the client to connect
            auto clientSocket = socket.accept();

            net::http::Request request = net::http::Request::read(clientSocket);

            target = request.target();
            method = request.method();

            for (auto header : request.headers()) {
                if (header.key() == "Content-Type") {
                    contentType = header.value();
                }

                if (header.key() == "Accept") {
                    acceptType = header.value();
                }
            }

            std::string answer(
                "HTTP/1.1 202 Accepted\r\nDate: Tue, 10 Sep 2019 09:03:26 "
                "GMT\r\nContent-Length: 0\r\n\r\n");

            ::send(clientSocket.handle(), answer.c_str(), answer.size(), 0);
        });

    started.get_future().wait();

    std::ostringstream oss;
    oss << "http://127.0.0.1:" << serverAddr.port() << "/api/traces";
    std::shared_ptr<opentracing::Tracer> tracer =
        ::jaegertracing::testutils::TracerUtil::buildTracer(oss.str());

    {
        auto span1 = tracer->StartSpan("tracedFunction");
    }

    serverThread.join();

    ASSERT_EQ(std::string("/api/traces?format=jaeger.thrift"), target);
    ASSERT_EQ(net::http::Method::POST, method);
    ASSERT_EQ(std::string("application/x-thrift"), contentType);
    ASSERT_EQ(std::string("application/x-thrift"), acceptType);
}

}  // namespace utils
}  // namespace jaegertracing
