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

#include "jaegertracing/utils/UDPTransporter.h"
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/protocol/TProtocol.h>

namespace jaegertracing {
namespace utils {

UDPTransporter::UDPTransporter(const net::IPAddress& serverAddr, int maxPacketSize)
    : Transport(maxPacketSize == 0 ? kUDPPacketMaxLength
                                   : maxPacketSize)
    , _buffer(new apache::thrift::transport::TMemoryBuffer(_maxPacketSize))
    , _serverAddr(serverAddr)
    , _client()
{
    using TProtocolFactory = apache::thrift::protocol::TProtocolFactory;
    using TCompactProtocolFactory =
        apache::thrift::protocol::TCompactProtocolFactory;

    _socket.open(AF_INET, SOCK_DGRAM);
    _socket.connect(_serverAddr);
    std::shared_ptr<TProtocolFactory> protocolFactory(
        new TCompactProtocolFactory());
    auto protocol = protocolFactory->getProtocol(_buffer);
    _client.reset(new agent::thrift::AgentClient(protocol));
}

}  // namespace utils
}  // namespace jaegertracing
