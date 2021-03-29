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

#ifndef JAEGERTRACING_TESTUTILS_TUDPTRANSPORT_H
#define JAEGERTRACING_TESTUTILS_TUDPTRANSPORT_H

#include "jaegertracing/Compilers.h"

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include <thrift/transport/TVirtualTransport.h>

#include "jaegertracing/utils/UDPTransporter.h"

namespace jaegertracing {
namespace testutils {

class TUDPTransport
    : public apache::thrift::transport::TVirtualTransport<TUDPTransport> {
  public:
    explicit TUDPTransport(const net::IPAddress& addr)
        : _socket()
        , _serverAddr(addr)
    {
        _socket.open(AF_INET, SOCK_DGRAM);
        _socket.bind(_serverAddr);
        if (_serverAddr.port() == 0) {
            ::sockaddr_storage addrStorage;
            ::socklen_t addrLen = sizeof(addrStorage);
            const auto returnCode =
                ::getsockname(_socket.handle(),
                              reinterpret_cast<::sockaddr*>(&addrStorage),
                              &addrLen);
            if (returnCode == 0) {
                _serverAddr = net::IPAddress(addrStorage, addrLen);
            }
        }
    }

    bool isOpen() override { return _socket.handle() >= 0; }

    void open() override {}

    void close() override { _socket.close(); }

    net::IPAddress addr() const { return _serverAddr; }

    uint32_t read(uint8_t* buf, uint32_t len)
    {
        ::sockaddr_storage clientAddr;
        auto clientAddrLen = static_cast<::socklen_t>(sizeof(clientAddr));
        const auto numRead =
            ::recvfrom(_socket.handle(),
#ifdef WIN32
                       reinterpret_cast<char*>(buf),  // this cast is safe
#else
                       buf,
#endif
                       len,
                       0,
                       reinterpret_cast<::sockaddr*>(&clientAddr),
                       &clientAddrLen);
        _clientAddr = net::IPAddress(clientAddr, clientAddrLen);
        // the return value conrreponds to the size of the data read from the
        // socket. upon error, recvfrom return -1. In this case, we return 0 to
        // say that noothing was read.
        return std::max<long>(0, numRead);
    }

    void write(const uint8_t* buf, uint32_t len)
    {
        ::sendto(_socket.handle(),
#ifdef WIN32
            reinterpret_cast<const char*>(buf),  // this cast is safe
#else
            buf,
#endif
            len,
            0,
            reinterpret_cast<const ::sockaddr*>(&_clientAddr.addr()),
            _clientAddr.addrLen());
    }

  private:
    net::Socket _socket;
    net::IPAddress _serverAddr;
    net::IPAddress _clientAddr;
    ::socklen_t _clientAddrLen;
};

}  // namespace testutils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_TESTUTILS_TUDPTRANSPORT_H
