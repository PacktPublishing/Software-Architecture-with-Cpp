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

#ifndef JAEGERTRACING_NET_SOCKET_H
#define JAEGERTRACING_NET_SOCKET_H

#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/net/URI.h"
#include <errno.h>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <system_error>

#ifdef WIN32
#include <iphlpapi.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET HandleType;

#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int HandleType;

#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4267)
#endif

namespace jaegertracing {
namespace net {



class Socket {
  public:
    Socket()
        : _handle(-1)
        , _family(-1)
        , _type(-1)
    {
    }

    Socket(const Socket&) = delete;

    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& socket)
        : _handle(socket._handle)
        , _family(socket._family)
        , _type(socket._type)
    {
        socket._handle = -1;
    }

    Socket& operator=(Socket&& rhs)
    {
        _handle = rhs._handle;
        _family = rhs._family;
        _type = rhs._type;
        return *this;
    }

    ~Socket()
    {
        close();
    }

    void open(int family, int type)
    {
        const auto handle = ::socket(family, type, 0);
        if (isHandleInvalid(handle)) {
            std::ostringstream oss;
            oss << "Failed to open socket"
                   ", family="
                << family << ", type=" << type;
            throw std::system_error(errno, std::system_category(), oss.str());
        }
        _handle = handle;
        _family = family;
        _type = type;
    }

    void bind(const IPAddress& addr)
    {
        const auto returnCode =
            ::bind(_handle,
                   reinterpret_cast<const ::sockaddr*>(&addr.addr()),
                   addr.addrLen());
        if (returnCode != 0) {
            std::ostringstream oss;
            oss << "Failed to bind socket to address"
                   ", addr=";
            addr.print(oss);
            throw std::system_error(errno, std::system_category(), oss.str());
        }
    }

    void bind(const std::string& ip, int port)
    {
        const auto addr = IPAddress::v4(ip, port);
        bind(addr);
    }

    void connect(const IPAddress& serverAddr)
    {
        const auto returnCode =
            ::connect(_handle,
                      reinterpret_cast<const ::sockaddr*>(&serverAddr.addr()),
                      serverAddr.addrLen());
        if (returnCode != 0) {
            std::ostringstream oss;
            oss << "Cannot connect socket to remote address ";
            serverAddr.print(oss);
            throw std::runtime_error(oss.str());
        }
    }

    IPAddress connect(const std::string& serverURIStr)
    {
        return connect(URI::parse(serverURIStr));
    }

    IPAddress connect(const URI& serverURI)
    {
        auto result =
            resolveAddress(serverURI._host, serverURI._port, AF_INET, _type);
        for (const auto* itr = result.get(); itr; itr = itr->ai_next) {
            const auto returnCode =
                ::connect(_handle, itr->ai_addr, itr->ai_addrlen);
            if (returnCode == 0) {
                return IPAddress(*itr->ai_addr, itr->ai_addrlen);
            }
        }
        std::ostringstream oss;
        oss << "Cannot connect socket to remote address ";
        serverURI.print(oss);
        throw std::runtime_error(oss.str());
    }

    static constexpr auto kDefaultBacklog = 128;

    void listen(int backlog = kDefaultBacklog)
    {
        const auto returnCode = ::listen(_handle, backlog);
        if (returnCode != 0) {
            throw std::system_error(
                errno, std::system_category(), "Failed to listen on socket");
        }
    }

    Socket accept()
    {
        ::sockaddr_storage addrStorage;
        ::socklen_t addrLen = sizeof(addrStorage);
        const auto clientHandle = ::accept(
            _handle, reinterpret_cast<::sockaddr*>(&addrStorage), &addrLen);

        if (isHandleInvalid(clientHandle)) {
            throw std::system_error(
                errno, std::system_category(), "Failed to accept on socket");
        }

        Socket clientSocket;
        clientSocket._handle = clientHandle;
        clientSocket._family =
            (addrLen == sizeof(::sockaddr_in)) ? AF_INET : AF_INET6;
        clientSocket._type = SOCK_STREAM;
        return clientSocket;
    }

    void close() noexcept
    {
        if (_handle >= 0) {
#ifdef WIN32
            ::closesocket(_handle);
#else
            ::close(_handle);
#endif
            _handle = -1;
        }
    }

    HandleType handle() { return _handle; }
  private:
    class OSResource {
      public:
        OSResource();
        ~OSResource();

        OSResource(const OSResource&) = delete;
        OSResource& operator=(const OSResource&) = delete;
        OSResource& operator=(const OSResource&&) = delete;
    };

    OSResource _osResource;
    HandleType _handle;
    int _family;
    int _type;

    static bool isHandleInvalid(HandleType h)
    {
#ifdef WIN32
        return (h == SOCKET_ERROR);
#else
        return (h < 0);
#endif
    }

    friend class IPAddress;
    friend std::unique_ptr<::addrinfo, AddrInfoDeleter>
    resolveAddress(const std::string& host, int port, int family, int type);
};

}  // namespace net
}  // namespace jaegertracing

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // JAEGERTRACING_NET_SOCKET_H
