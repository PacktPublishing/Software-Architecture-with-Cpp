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
#include "jaegertracing/platform/Hostname.h"
#include "jaegertracing/net/Socket.h"

#include <sys/types.h>

#ifndef WIN32
#include <ifaddrs.h>
#endif

namespace jaegertracing {
namespace net {

namespace {

template <typename T>
struct CDeleter : public std::function<void(T*)> {
    void operator()(T* ifAddr) const
    {
#ifdef WIN32
        free(ifAddr);
#else
        if (ifAddr) {
            ::freeifaddrs(ifAddr);
        }
#endif
    }
};

}  // anonymous namespace

#ifdef WIN32

IPAddress _localIP(int family)
{
    DWORD size = 15032;
    DWORD rv = GetAdaptersAddresses(
        AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
    if (rv != ERROR_BUFFER_OVERFLOW) {
        // GetAdaptersAddresses() failed...
        return IPAddress();
    }
    std::unique_ptr<IP_ADAPTER_ADDRESSES, CDeleter<IP_ADAPTER_ADDRESSES>>
        adapter_addresses((IP_ADAPTER_ADDRESSES*)malloc(size));

    rv = GetAdaptersAddresses(
        AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, &*adapter_addresses, &size);
    if (rv != ERROR_SUCCESS) {
        // GetAdaptersAddresses() failed...
        return IPAddress();
    }

    for (PIP_ADAPTER_ADDRESSES aa = &*adapter_addresses; aa != NULL;
         aa = aa->Next) {
        for (PIP_ADAPTER_UNICAST_ADDRESS ua = aa->FirstUnicastAddress;
             ua != NULL;
             ua = ua->Next) {
            if (family == ua->Address.lpSockaddr->sa_family) {
                char buf[BUFSIZ] = { 0 };
                getnameinfo(ua->Address.lpSockaddr,
                            ua->Address.iSockaddrLength,
                            buf,
                            sizeof(buf),
                            NULL,
                            0,
                            NI_NUMERICHOST);

                return IPAddress(*(ua->Address.lpSockaddr),
                                 (family == AF_INET) ? sizeof(::sockaddr_in)
                                                     : sizeof(::sockaddr_in6));
            }
        }
    }

    return IPAddress();
}

#else

namespace {

IPAddress _localIP(std::function<bool(const ifaddrs*)> filter)
{
    auto* ifAddrRawPtr = static_cast<ifaddrs*>(nullptr);
    getifaddrs(&ifAddrRawPtr);
    std::unique_ptr<ifaddrs, CDeleter<ifaddrs>> ifAddr(ifAddrRawPtr);
    for (auto* itr = ifAddr.get(); itr; itr = itr->ifa_next) {
        if (filter(itr)) {
            const auto family = ifAddr->ifa_addr->sa_family;
            const auto addrLen = (family == AF_INET) ? sizeof(::sockaddr_in)
                                                     : sizeof(::sockaddr_in6);
            return IPAddress(*itr->ifa_addr, addrLen);
        }
    }
    return IPAddress();
}

IPAddress _localIP(int family)
{
    return _localIP([family](const ifaddrs* ifAddr) {
        return ifAddr->ifa_addr != nullptr &&
               ifAddr->ifa_addr->sa_family == family;
    });
}


} // anonymous namespace

#endif

IPAddress IPAddress::localIP(int family)
{
    try {
        return versionFromString(platform::hostname(), 0, family);
    } catch (...) {
        // Fall back to returning the first matching interface
    }
    return _localIP(family);
}

IPAddress
IPAddress::versionFromString(const std::string& ip, int port, int family)
{
    ::sockaddr_storage addrStorage;
    std::memset(&addrStorage, 0, sizeof(addrStorage));

    auto* addrBuffer = static_cast<void*>(nullptr);
    if (family == AF_INET) {
        ::sockaddr_in& addr = *reinterpret_cast<::sockaddr_in*>(&addrStorage);
        addr.sin_family = family;
        addr.sin_port = htons(port);
        addrBuffer = &addr.sin_addr;
    }
    else {
        assert(family == AF_INET6);
        ::sockaddr_in6& addr = *reinterpret_cast<::sockaddr_in6*>(&addrStorage);
        addr.sin6_family = family;
        addr.sin6_port = htons(port);
        addrBuffer = &addr.sin6_addr;
    }

    const auto returnCode = inet_pton(family, ip.c_str(), addrBuffer);

    if (returnCode == 0) {
        auto result = resolveAddress(ip, port, family);
        assert(result);
        std::memcpy(&addrStorage, result->ai_addr, result->ai_addrlen);
    }
    return IPAddress(addrStorage,
                     family == AF_INET ? sizeof(::sockaddr_in)
                                       : sizeof(::sockaddr_in6));
}

std::unique_ptr<::addrinfo, AddrInfoDeleter>
resolveAddress(const std::string& host, int port, int family, int type)
{
// On windows, getaddrinfo does not return an error for empty host
#ifdef WIN32
    if (host.empty()) {
        throw std::runtime_error("Error resolving address: ");
    }
#endif
    ::addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = type;

    std::string service;
    if (port != 0) {
        service = std::to_string(port);
    }

    Socket::OSResource osResouce;

    auto* servInfoPtr = static_cast<::addrinfo*>(nullptr);
    const auto returnCode =
        getaddrinfo(host.c_str(), service.c_str(), &hints, &servInfoPtr);
    std::unique_ptr<::addrinfo, AddrInfoDeleter> servInfo(servInfoPtr);

    if (returnCode != 0) {
        std::ostringstream oss;
        oss << "Error resolving address: " << gai_strerror(returnCode);
        throw std::runtime_error(oss.str());
    }

    return servInfo;
}

}  // namespace net
}  // namespace jaegertracing
