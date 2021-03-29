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

#ifndef JAEGERTRACING_UTILS_UDPCLIENT5_H
#define JAEGERTRACING_UTILS_UDPCLIENT5_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <system_error>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/utils/Transport.h"

#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/http/Response.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/THttpClient.h>

namespace jaegertracing {
namespace utils {

class HTTPTransporter : public Transport {
  public:
    HTTPTransporter(const net::URI& endpoint, int maxPacketSize);

    ~HTTPTransporter() { close(); }

    void emitBatch(const thrift::Batch& batch) override
    {
        // Resets the buffer to write a new batch
        _buffer->resetBuffer();

        // Does the serialisation to Thrift
        auto oprot = _protocol.get();
        batch.write(oprot);
        oprot->writeMessageEnd();
        oprot->getTransport()->writeEnd();
        oprot->getTransport()->flush();

        uint8_t* data = nullptr;
        uint32_t size = 0;
        _buffer->getBuffer(&data, &size);

        // Sends the HTTP message
        const auto numWritten = ::send(_socket.handle(), reinterpret_cast<char*>(data), sizeof(uint8_t) * size, 0);

        if (static_cast<unsigned>(numWritten) != size) {
            std::ostringstream oss;
            oss << "Failed to write message, numWritten=" << numWritten << ", size=" << size;
            throw std::system_error(errno, std::system_category(), oss.str());
        }

        // Waits for response. Check that the server acknowledged
        // and returned a green status [200, 201, 202, 203, 204]
        net::http::Response response = net::http::read(_socket);
        if (response.statusCode() < 200 && response.statusCode() > 204) {
          std::ostringstream oss;
          oss << "Failed to write message, HTTP error " << response.statusCode() << response.reason();
          throw std::system_error(errno, std::system_category(), oss.str());
        }
    }

    std::unique_ptr<apache::thrift::protocol::TProtocolFactory>
    protocolFactory() const override
    {
        return std::unique_ptr<apache::thrift::protocol::TProtocolFactory>(
            new apache::thrift::protocol::TBinaryProtocolFactory());
    }

  private:
    std::shared_ptr<apache::thrift::transport::TMemoryBuffer> _buffer;
    net::IPAddress _serverAddr;
    std::shared_ptr<::apache::thrift::transport::THttpClient> _httpClient;
    std::shared_ptr<apache::thrift::protocol::TProtocol> _protocol;

    static constexpr auto kHttpPacketMaxLength = 1024 * 1024; // 1MB
};

}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_UTILS_UDPCLIENT_H
