/*
 * Copyright (c) 2019, The Jaeger Authors
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

#include "jaegertracing/utils/HTTPTransporter.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TProtocol.h>

namespace jaegertracing {
namespace utils {

HTTPTransporter::HTTPTransporter(const net::URI& endpoint, int maxPacketSize)
    : Transport(maxPacketSize == 0 ? kHttpPacketMaxLength : maxPacketSize)
    , _buffer(new apache::thrift::transport::TMemoryBuffer(_maxPacketSize))
    , _serverAddr(net::IPAddress::v4(endpoint._host, endpoint._port))
    , _httpClient(new ::apache::thrift::transport::THttpClient(
        _buffer, endpoint._host, endpoint._path + "?format=jaeger.thrift"))
{
    using TProtocolFactory = apache::thrift::protocol::TProtocolFactory;
    using TBinaryProtocolFactory =
        apache::thrift::protocol::TBinaryProtocolFactory;

    _socket.open(AF_INET, SOCK_STREAM);
    _socket.connect(_serverAddr);
    std::shared_ptr<TProtocolFactory> protocolFactory(
        new TBinaryProtocolFactory());
    _protocol = protocolFactory->getProtocol(_httpClient);
}

}  // namespace utils
}  // namespace jaegertracing
