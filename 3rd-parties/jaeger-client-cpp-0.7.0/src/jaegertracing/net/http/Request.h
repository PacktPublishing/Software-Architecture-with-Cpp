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

#ifndef JAEGERTRACING_NET_HTTP_REQUEST_H
#define JAEGERTRACING_NET_HTTP_REQUEST_H

#include "jaegertracing/net/URI.h"
#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/http/Error.h"
#include "jaegertracing/net/http/Header.h"
#include "jaegertracing/net/http/Method.h"

namespace jaegertracing {
namespace net {
namespace http {

class Request {
  public:
    static Request parse(std::istream& in);

    static Request read(Socket& in);

    Request()
        : _method()
        , _target()
        , _version()
        , _headers()
    {
    }

    Method method() const { return _method; }

    const std::string& target() const { return _target; }

    const std::string& version() const { return _version; }

    const std::vector<Header>& headers() const { return _headers; }

    const std::string& body() const { return _body; }

  private:
    Method _method;
    std::string _target;
    std::string _version;
    std::vector<Header> _headers;
    std::string _body;
};

}  // namespace http
}  // namespace net
}  // namespace jaegertracing

#endif  // JAEGERTRACING_NET_HTTP_REQUEST_H
