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

#ifndef JAEGERTRACING_NET_IPADDRESS_H
#define JAEGERTRACING_NET_IPADDRESS_H


#include <array>
#include <cassert>
#include <cstring>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <vector>

#include "jaegertracing/Compilers.h"

#ifdef WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h> 
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif




struct ifaddrs;

namespace jaegertracing {
namespace net {

class IPAddress {
  public:
    static std::pair<std::string, int> parse(const std::string& hostPort)
    {
        const auto colonPos = hostPort.find(':');
        const auto ip = hostPort.substr(0, colonPos);
        int port = 0;
        if (colonPos != std::string::npos) {
            const auto portStr = hostPort.substr(colonPos + 1);
            std::istringstream iss(portStr);
            if (!(iss >> port)) {
                port = 0;
            }
        }
        return std::make_pair(ip, port);
    }

    static IPAddress v4(const std::string& hostPort)
    {
        auto result = parse(hostPort);
        return v4(result.first, result.second);
    }

    static IPAddress v4(const std::string& ip, int port)
    {
        return versionFromString(ip, port, AF_INET);
    }

    static IPAddress v6(const std::string& ip, int port)
    {
        return versionFromString(ip, port, AF_INET6);
    }

    static IPAddress localIP(int family);

    IPAddress()
        : _addr()
        , _addrLen(sizeof(::sockaddr_in))
    {
        std::memset(&_addr, 0, sizeof(_addr));
    }

    IPAddress(const ::sockaddr_storage& addr, ::socklen_t addrLen)
        : _addr(addr)
        , _addrLen(addrLen)
    {
    }

    IPAddress(const ::sockaddr& addr, ::socklen_t addrLen)
        : IPAddress()
    {
        std::memcpy(&_addr, &addr, addrLen);
    }

    explicit IPAddress(const ::sockaddr_in& addr)
        : IPAddress(reinterpret_cast<const ::sockaddr&>(addr), sizeof(addr))
    {
    }

    explicit IPAddress(const ::sockaddr_in6& addr)
        : IPAddress(reinterpret_cast<const ::sockaddr&>(addr), sizeof(addr))
    {
    }

    bool operator==(const IPAddress& rhs) const
    {
        if (_addrLen != rhs._addrLen) {
            return false;
        }
        return std::memcmp(&_addr, &rhs._addr, _addrLen) == 0;
    }

    const ::sockaddr_storage& addr() const { return _addr; }

    ::socklen_t addrLen() const { return _addrLen; }

    void print(std::ostream& out) const
    {
        out << "{ family=" << family();
        const auto addrStr = host();
        if (!addrStr.empty()) {
            out << ", addr=" << addrStr;
        }
        out << ", port=" << port() << " }";
    }

    std::string authority() const
    {
        const auto portNum = port();
        if (portNum != 0) {
            return host() + ':' + std::to_string(portNum);
        }
        return host();
    }

    std::string host() const
    {
        std::array<char, INET6_ADDRSTRLEN> buffer;
        const auto af = family();
        const auto* addrStr = ::inet_ntop(
            af,
            af == AF_INET
                ? const_cast<void*>(
                    static_cast<const void*>(
                        &reinterpret_cast<const ::sockaddr_in&>(_addr).sin_addr))
                : const_cast<void*>(
                    static_cast<const void*>(
                        &reinterpret_cast<const ::sockaddr_in6&>(_addr).sin6_addr)),
            &buffer[0],
            buffer.size());
        return addrStr ? addrStr : "";
    }

    int port() const
    {
        if (family() == AF_INET) {
            return ntohs(
                reinterpret_cast<const ::sockaddr_in&>(_addr).sin_port);
        }
        return ntohs(reinterpret_cast<const ::sockaddr_in6&>(_addr).sin6_port);
    }

    int family() const
    {
        if (_addrLen == sizeof(::sockaddr_in)) {
            return AF_INET;
        }
        assert(_addrLen == sizeof(::sockaddr_in6));
        return AF_INET6;
    }

  private:
    static IPAddress versionFromString(const std::string& ip, int port, int family);

    ::sockaddr_storage _addr;
    ::socklen_t _addrLen;
};

struct AddrInfoDeleter : public std::function<void(::addrinfo*)> {
    void operator()(::addrinfo* addrInfo) const { ::freeaddrinfo(addrInfo); }
};

std::unique_ptr<::addrinfo, AddrInfoDeleter> resolveAddress(
    const std::string& host, int port, int family, int type = SOCK_STREAM);

}  // namespace net
}  // namespace jaegertracing

inline std::ostream& operator<<(std::ostream& out,
                                const jaegertracing::net::IPAddress& addr)
{
    addr.print(out);
    return out;
}

#endif  // JAEGERTRACING_NET_IPADDRESS_H
