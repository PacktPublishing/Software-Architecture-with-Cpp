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

#ifndef JAEGERTRACING_UTILS_UDPCLIENT_H
#define JAEGERTRACING_UTILS_UDPCLIENT_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <system_error>

#include "jaegertracing/Compilers.h"

#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TBufferTransports.h>

#include "jaegertracing/utils/Transport.h"

#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/thrift-gen/Agent.h"

namespace jaegertracing {
namespace utils {

class UDPTransporter : public Transport {
  public:
    static constexpr auto kUDPPacketMaxLength = 65000;

    UDPTransporter(const net::IPAddress& serverAddr, int maxPacketSize);

    void emitZipkinBatch(
        const std::vector<twitter::zipkin::thrift::Span>& spans)
    {
        throw std::logic_error("emitZipkinBatch not implemented");
    }

    void emitBatch(const thrift::Batch& batch) override
    {
        _buffer->resetBuffer();
        _client->emitBatch(batch);
        uint8_t* data = nullptr;
        uint32_t size = 0;
        _buffer->getBuffer(&data, &size);
        if (static_cast<int>(size) > _maxPacketSize) {
            std::ostringstream oss;
            oss << "Data does not fit within one UDP packet"
                   ", size "
                << size << ", max " << _maxPacketSize << ", spans "
                << batch.spans.size();
            throw std::logic_error(oss.str());
        }
        const auto numWritten = ::send(_socket.handle(), reinterpret_cast<char*>(data), sizeof(uint8_t) * size, 0);
        if (static_cast<unsigned>(numWritten) != size) {
            std::ostringstream oss;
            oss << "Failed to write message"
                   ", numWritten="
                << numWritten << ", size=" << size;
            throw std::system_error(errno, std::system_category(), oss.str());
        }
    }

  std::unique_ptr< apache::thrift::protocol::TProtocolFactory > protocolFactory() const override {
    return std::unique_ptr<apache::thrift::protocol::TProtocolFactory>(new apache::thrift::protocol::TCompactProtocolFactory());
  }

  private:
    std::shared_ptr<apache::thrift::transport::TMemoryBuffer> _buffer;
    net::IPAddress _serverAddr;
    std::unique_ptr<agent::thrift::AgentClient> _client;
};

}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_UTILS_UDPCLIENT_H
