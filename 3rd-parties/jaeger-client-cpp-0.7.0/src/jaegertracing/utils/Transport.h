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

#ifndef JAEGERTRACING_UTILS_SENDER_H
#define JAEGERTRACING_UTILS_SENDER_H

#include "jaegertracing/net/Socket.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"

namespace jaegertracing {
namespace utils {

class Transport {
  public:
    Transport(int maxPacketSize)
        : _maxPacketSize(maxPacketSize)
    {
    }

    virtual ~Transport() { close(); }

    virtual void emitBatch(const thrift::Batch& batch) = 0;

    int maxPacketSize() const { return _maxPacketSize; }

    void close() { _socket.close(); }

    virtual std::unique_ptr<apache::thrift::protocol::TProtocolFactory>
    protocolFactory() const = 0;

  protected:
    int _maxPacketSize;
    net::Socket _socket;
};

}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_UTILS_SENDER_H
