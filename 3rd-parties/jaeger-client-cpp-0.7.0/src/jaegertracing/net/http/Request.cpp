/*
 * Copyright (c) 2017-2018 Uber Technologies, Inc.
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

#include "jaegertracing/net/http/Request.h"
#include "jaegertracing/net/http/SocketReader.h"

#include <regex>

namespace jaegertracing {
namespace net {
namespace http {

Request Request::read(Socket & socket)
{
  std::string response = SocketReader::read(socket);
  std::istringstream responseStream(response);
  return Request::parse(responseStream);
}

Request Request::parse(std::istream& in)
{
    const std::regex requestLinePattern(
        "([A-Z]+) ([^ ]+) HTTP/([0-9]\\.[0-9])$");
    std::string line;
    std::smatch match;
    if (!readLineCRLF(in, line) ||
        !std::regex_match(line, match, requestLinePattern) ||
        match.size() < 4) {
        throw ParseError::make("request line", line);
    }
    Request request;

    request._method = parseMethod(match[1]);
    request._target = match[2];
    request._version = match[3];

    readHeaders(in, request._headers);
    request._body = std::string(std::istreambuf_iterator<char>(in),
                                std::istreambuf_iterator<char>{});

    return request;
}

}  // namespace http
}  // namespace net
}  // namespace jaegertracing
