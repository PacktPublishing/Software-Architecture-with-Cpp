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

#include <gtest/gtest.h>

#include "jaegertracing/Tracer.h"
#include "jaegertracing/ThriftSender.h"
#include "jaegertracing/testutils/TracerUtil.h"
#include "jaegertracing/utils/ErrorUtil.h"

namespace jaegertracing {
namespace {

class MockUDPSender : public utils::UDPTransporter {
  public:
    enum class ExceptionType { kSystemError, kException, kString };

    MockUDPSender(const net::IPAddress& serverAddr,
                  int maxPacketSize,
                  ExceptionType type)
        : UDPTransporter(serverAddr, maxPacketSize)
        , _type(type)
    {
    }

  private:
    void emitBatch(const thrift::Batch& batch) override
    {
        switch (_type) {
        case ExceptionType::kSystemError:
            throw std::system_error(
                std::make_error_code(std::errc::invalid_argument));
        case ExceptionType::kException:
            throw std::exception();
        default:
            assert(_type == ExceptionType::kString);
            throw "error";
        }
    }

    ExceptionType _type;
};

class MockThriftSender : public ThriftSender {
  public:
    MockThriftSender(const net::IPAddress& ip,
                     int maxPacketSize,
                     MockUDPSender::ExceptionType type)
        : ThriftSender(std::unique_ptr<utils::Transport>(new MockUDPSender(ip, maxPacketSize, type)))
    {
    }
};

}  // anonymous namespace

TEST(ThriftSender, testManyMessages)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer =
        std::static_pointer_cast<const Tracer>(opentracing::Tracer::Global());

    std::unique_ptr<utils::Transport> transporter(
        new utils::UDPTransporter(handle->_mockAgent->spanServerAddress(), 9216));
    ThriftSender sender(
        std::forward<std::unique_ptr<utils::Transport>>(transporter));
    constexpr auto kNumMessages = 2000;
    const auto logger = logging::consoleLogger();
    for (auto i = 0; i < kNumMessages; ++i) {
        Span span(tracer);
        span.SetOperationName("test" + std::to_string(i));
        ASSERT_NO_THROW(sender.append(span));
    }
}

TEST(ThriftSender, testExceptions)
{
    const auto handle = testutils::TracerUtil::installGlobalTracer();
    const auto tracer =
        std::static_pointer_cast<const Tracer>(opentracing::Tracer::Global());

    Span span(tracer);
    span.SetOperationName("test");

    const MockUDPSender::ExceptionType exceptionTypes[] = {
        MockUDPSender::ExceptionType::kSystemError,
        MockUDPSender::ExceptionType::kException,
        MockUDPSender::ExceptionType::kString
    };
    for (auto type : exceptionTypes) {
      MockThriftSender mockSender(net::IPAddress::v4("localhost", 1234), 0, type);
      mockSender.append(span);
        ASSERT_THROW(mockSender.flush(), Sender::Exception);
    }
}

}  // namespace jaegertracing
