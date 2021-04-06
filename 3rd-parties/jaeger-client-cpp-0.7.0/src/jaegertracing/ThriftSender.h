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

#ifndef JAEGERTRACING_THRIFTSENDER_H
#define JAEGERTRACING_THRIFTSENDER_H

#include "jaegertracing/Compilers.h"
#include "jaegertracing/Span.h"
#include "jaegertracing/Sender.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"
#include "jaegertracing/utils/Transport.h"
#include <thrift/transport/TBufferTransports.h>

namespace jaegertracing {

class ThriftSender : public Sender {
  public:
    ThriftSender(std::unique_ptr<utils::Transport>&& transporter);

    ~ThriftSender() { close(); }

    int append(const Span& span) override;

    int flush() override;

    void close() override { _transporter->close(); }

  protected:
    void setClient(std::unique_ptr<utils::Transport>&& client)
    {
      _transporter = std::move(client);
    }

  private:
    void resetBuffers()
    {
        _spanBuffer.clear();
        _byteBufferSize = _processByteSize;
    }

    template <typename ThriftType>
    int calcSizeOfSerializedThrift(const ThriftType& base)
    {
        _thriftBuffer->resetBuffer();
        auto _protocol = _protocolFactory->getProtocol(_thriftBuffer);
        base.write(_protocol.get());
        uint8_t* data = nullptr;
        uint32_t size = 0;
        _thriftBuffer->getBuffer(&data, &size);
        return size;
    }

    std::unique_ptr<utils::Transport> _transporter;
    int _maxSpanBytes;
    int _byteBufferSize;
    std::vector<thrift::Span> _spanBuffer;
    thrift::Process _process;
    int _processByteSize;
    std::unique_ptr<apache::thrift::protocol::TProtocolFactory> _protocolFactory;
    // reuse buffer across serializations of different ThriftType for size
    // calculation
    std::shared_ptr<apache::thrift::transport::TMemoryBuffer> _thriftBuffer;
};

}  // namespace jaegertracing

#endif  //JAEGERTRACING_THRIFTSENDER_H
