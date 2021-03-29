/*
 * Copyright (c) 2018, The Jaeger Authors.
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

#ifndef JAEGERTRACING_NET_HTTP_SOCKET_READER_H
#define JAEGERTRACING_NET_HTTP_SOCKET_READER_H

#include "jaegertracing/net/Socket.h"

namespace jaegertracing {
namespace net {
namespace http {

class SocketReader {
  public:
    static std::string read(Socket& socket) {
      constexpr auto kBufferSize = 1024;
      std::array<char, kBufferSize> buffer{};
      auto numRead = ::recv(socket.handle(), &buffer[0], buffer.size(), 0);
      std::string response;
      while (numRead > 0) {
        response.append(&buffer[0], numRead);
        if (numRead < static_cast<int>(buffer.size())) {
          break;
        }
        numRead = ::recv(socket.handle(), &buffer[0], buffer.size(), 0);
      }
      return response;
    }
};

}  // namespace http
}  // namespace net
}  // namespace jaegertracing

#endif  // JAEGERTRACING_NET_HTTP_SOCKET_READER_H
