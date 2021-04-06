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

#include "jaegertracing/net/Socket.h"

#ifdef WIN32

#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#endif

namespace jaegertracing {
namespace net {

namespace {

    
void initSocket()
{
#ifdef WIN32
    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    WORD wVersionRequested = MAKEWORD(2, 2);

    WSADATA wsaData;
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        std::ostringstream oss;
        oss << "Failed to find a usable Winsock DLL. WSAStartup failed with "
               "error "
            << err;
        throw std::system_error(errno, std::system_category(), oss.str());
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();

        std::ostringstream oss;
        oss << "Failed to find a usable Winsock DLL. WSAStartup failed with "
               "error "
            << err;
        throw std::system_error(errno, std::system_category(), oss.str());
    }
#endif
}

void cleanSocket()
{
#ifdef WIN32
    WSACleanup();
#endif
}

}


Socket::OSResource::OSResource() { initSocket(); }

Socket::OSResource::~OSResource() { cleanSocket(); }


}  // namespace net
}  // namespace jaegertracing
