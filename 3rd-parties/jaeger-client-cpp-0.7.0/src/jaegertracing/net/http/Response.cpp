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

#include "jaegertracing/net/http/Response.h"
#include "jaegertracing/net/http/SocketReader.h"

#include <regex>
#include <sstream>
#include <stdexcept>

#include "jaegertracing/Constants.h"
#include "jaegertracing/net/Socket.h"

#ifdef _MSC_VER
#pragma warning(disable : 4267)
#endif

namespace jaegertracing {
namespace net {
namespace http {

Response Response::parse(std::istream& in)
{
    const std::regex statusLinePattern("HTTP/([0-9]\\.[0-9]) ([0-9]+) (.+)$");
    std::string line;
    std::smatch match;
    if (!readLineCRLF(in, line) ||
        !std::regex_match(line, match, statusLinePattern) || match.size() < 4) {
        throw ParseError::make("status line", line);
    }
    Response response;
    response._version = match[1];
    std::istringstream iss(match[2]);
    iss >> response._statusCode;
    response._reason = match[3];

    readHeaders(in, response._headers);

    response._body = std::string(std::istreambuf_iterator<char>(in),
                                 std::istreambuf_iterator<char>{});

    return response;
}

Response read(Socket & socket)
{
  std::string response = SocketReader::read(socket);
  std::istringstream responseStream(response);
  return Response::parse(responseStream);
}

Response get(const URI& uri)
{
    Socket socket;
    socket.open(AF_INET, SOCK_STREAM);
    socket.connect(uri);
    std::ostringstream requestStream;
    requestStream << "GET " << uri.target() << " HTTP/1.1\r\n"
                  << "Host: " << uri.authority()
                  << "\r\n"
                     "User-Agent: jaegertracing/"
                  << kJaegerClientVersion << "\r\n\r\n";
    const auto request = requestStream.str();
#ifdef WIN32
    const auto numWritten = ::send(socket.handle(), request.c_str(), request.size(), 0);
#else
    const auto numWritten = ::write(socket.handle(), request.c_str(), request.size());
#endif
    if (numWritten != static_cast<int>(request.size())) {
        std::ostringstream oss;
        oss << "Failed to write entire HTTP request"
            << ", uri=" << uri << ", request=" << request;
        throw std::system_error(errno, std::system_category(), oss.str());
    }

  return read(socket);
}

}  // namespace http
}  // namespace net
}  // namespace jaegertracing
