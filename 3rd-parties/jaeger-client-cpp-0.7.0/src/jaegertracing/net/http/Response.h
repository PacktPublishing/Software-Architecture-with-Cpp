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

#ifndef JAEGERTRACING_NET_HTTP_RESPONSE_H
#define JAEGERTRACING_NET_HTTP_RESPONSE_H

#include "jaegertracing/net/URI.h"
#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/http/Error.h"
#include "jaegertracing/net/http/Header.h"
#include "jaegertracing/net/http/Method.h"

namespace jaegertracing {
namespace net {
namespace http {

class Response {
  public:
    static Response parse(std::istream& in);

    Response()
        : _version()
        , _statusCode(0)
        , _reason()
        , _headers()
        , _body()
    {
    }

    int statusCode() const { return _statusCode; }

    const std::string& reason() const { return _reason; }

    const std::vector<Header>& headers() const { return _headers; }

    const std::string& body() const { return _body; }

  private:
    std::string _version;
    int _statusCode;
    std::string _reason;
    std::vector<Header> _headers;
    std::string _body;
};

Response get(const URI& uri);

/**
 * Reads http response from a socket
 */
Response read(Socket& socket);

}  // namespace http
}  // namespace net
}  // namespace jaegertracing

#endif  // JAEGERTRACING_NET_HTTP_RESPONSE_H
