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

#ifndef JAEGERTRACING_CROSSDOCK_SERVER_H
#define JAEGERTRACING_CROSSDOCK_SERVER_H

#include <memory>

#include <opentracing/tracer.h>

#include "jaegertracing/thrift-gen/tracetest_types.h"

namespace jaegertracing {
namespace logging {

class Logger;

}  // namespace logging

namespace net {

class IPAddress;

namespace http {

class Request;

}  // namespace http
}  // namespace net

namespace crossdock {

class Server {
  public:
    Server(const net::IPAddress& clientIP,
           const net::IPAddress& serverIP,
           const std::string& agentHostPort,
           const std::string& collectorEndpoint,
           const std::string& samplingServerURL);

    ~Server();

    void serve();

  private:
    template <typename RequestType>
    std::string handleJSON(
        const net::http::Request& request,
        std::function<thrift::TraceResponse(
            const RequestType&, const opentracing::SpanContext&)> handler);

    std::string handleRequest(const net::http::Request& request);

    thrift::TraceResponse startTrace(const thrift::StartTraceRequest& request);

    thrift::TraceResponse joinTrace(const thrift::JoinTraceRequest& request,
                                    const opentracing::SpanContext& ctx);

    std::string generateTraces(const net::http::Request& request);

    class SocketListener;
    class EndToEndHandler;

    std::shared_ptr<logging::Logger> _logger;
    std::shared_ptr<opentracing::Tracer> _tracer;
    std::unique_ptr<SocketListener> _clientListener;
    std::unique_ptr<SocketListener> _serverListener;
    std::unique_ptr<EndToEndHandler> _handler;
};

}  // namespace crossdock
}  // namespace jaegertracing

#endif  // JAEGERTRACING_CROSSDOCK_SERVER_H
