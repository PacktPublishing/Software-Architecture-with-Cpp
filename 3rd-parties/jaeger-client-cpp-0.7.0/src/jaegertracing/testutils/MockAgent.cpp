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

#include "jaegertracing/testutils/MockAgent.h"

#include <regex>
#include <thread>

#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TBufferTransports.h>

#include "jaegertracing/Logging.h"
#include "jaegertracing/baggage/RemoteRestrictionJSON.h"
#include "jaegertracing/net/http/Error.h"
#include "jaegertracing/net/http/Request.h"
#include "jaegertracing/net/http/Response.h"
#include "jaegertracing/samplers/RemoteSamplingJSON.h"
#include "jaegertracing/utils/ErrorUtil.h"
#include "jaegertracing/utils/UDPTransporter.h"

#ifdef _MSC_VER
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#endif

namespace jaegertracing {
namespace testutils {
namespace {

#ifdef WIN32
#define READ_ERROR SOCKET_ERROR
#else
#define READ_ERROR -1
#endif

static size_t read(int socketHandle, char* buffer, size_t size)
{
    int returnValue = ::recv(socketHandle, buffer, size, 0);
    return (returnValue == READ_ERROR) ? 0 : returnValue;
}

bool startsWith(const std::string& str, const std::string& prefix)
{
    if (str.size() < prefix.size()) {
        return false;
    }
    return std::equal(std::begin(prefix), std::end(prefix), std::begin(str));
}

}  // anonymous namespace

MockAgent::~MockAgent() { close(); }

void MockAgent::start()
{
    std::promise<void> startedUDP;
    std::promise<void> startedHTTP;
    _udpThread = std::thread([this, &startedUDP]() { serveUDP(startedUDP); });
    _httpThread =
        std::thread([this, &startedHTTP]() { serveHTTP(startedHTTP); });
    startedUDP.get_future().wait();
    startedHTTP.get_future().wait();
}

void MockAgent::close()
{
    if (_servingUDP) {
        _servingUDP = false;
        _transport.close();
        _udpThread.join();
    }

    if (_servingHTTP) {
        _servingHTTP = false;
        _httpThread.join();
    }
}

void MockAgent::emitBatch(const thrift::Batch& batch)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _batches.push_back(batch);
}

MockAgent::MockAgent()
    : _transport(net::IPAddress::v4("127.0.0.1", 0))
    , _servingUDP(false)
{
}

void MockAgent::serveUDP(std::promise<void>& started)
{
    using TCompactProtocolFactory =
        apache::thrift::protocol::TCompactProtocolFactory;
    using TMemoryBuffer = apache::thrift::transport::TMemoryBuffer;

    auto iface = shared_from_this();
    agent::thrift::AgentProcessor handler(iface);
    TCompactProtocolFactory protocolFactory;
    std::shared_ptr<TMemoryBuffer> trans(
        new TMemoryBuffer(utils::UDPTransporter::kUDPPacketMaxLength));

    // Notify main thread that setup is done.
    _servingUDP = true;
    started.set_value();

    std::array<uint8_t, utils::UDPTransporter::kUDPPacketMaxLength> buffer;
    while (isServingUDP()) {
        try {
            const auto numRead =
                _transport.read(&buffer[0], utils::UDPTransporter::kUDPPacketMaxLength);
            if (numRead > 0) {
                trans->write(&buffer[0], numRead);
                auto protocol = protocolFactory.getProtocol(trans);
                handler.process(protocol, protocol, nullptr);
            }
        } catch (...) {
            auto logger = logging::consoleLogger();
            utils::ErrorUtil::logError(
                *logger, "An error occurred in MockAgent::serveUDP");
        }
    }
}

void MockAgent::serveHTTP(std::promise<void>& started)
{
    net::Socket socket;
    socket.open(AF_INET, SOCK_STREAM);
    socket.bind(net::IPAddress::v4("127.0.0.1", 0));
    socket.listen();
    ::sockaddr_storage addrStorage;
    ::socklen_t addrLen = sizeof(addrStorage);
    const auto returnCode = ::getsockname(
        socket.handle(), reinterpret_cast<sockaddr*>(&addrStorage), &addrLen);
    if (returnCode != 0) {
        throw std::system_error(errno,
                                std::generic_category(),
                                "Failed to get HTTP address from socket");
    }
    _httpAddress = net::IPAddress(addrStorage, addrLen);

    _servingHTTP = true;
    started.set_value();

    const std::regex servicePattern("[?&]service=([^?&]+)");
    while (isServingHTTP()) {
        constexpr auto kBufferSize = 256;
        std::array<char, kBufferSize> buffer;
        std::string requestStr;
        auto clientSocket = socket.accept();
        auto numRead = read(
            clientSocket.handle(), &buffer[0], buffer.size());
        while (numRead > 0) {
            requestStr.append(&buffer[0], numRead);
            if (numRead < static_cast<int>(buffer.size())) {
                break;
            }
            numRead = read(
                clientSocket.handle(), &buffer[0], buffer.size());
        }

        try {
            enum class Resource { kSampler, kBaggage };

            std::istringstream iss(requestStr);
            const auto request = net::http::Request::parse(iss);
            const auto target = request.target();

            auto resource = Resource::kSampler;
            if (startsWith(target, "/baggageRestrictions") ||
                startsWith(target,
                           _httpAddress.authority() + "/baggageRestrictions")) {
                resource = Resource::kBaggage;
            }
            std::smatch match;
            if (!std::regex_search(target, match, servicePattern)) {
                throw net::http::ParseError("no 'service' parameter");
            }
            if (std::regex_search(match.suffix().str(), servicePattern)) {
                throw net::http::ParseError(
                    "'service' parameter must occur only once");
            }
            const auto serviceName = match[1].str();

            std::string responseJSON;
            switch (resource) {
            case Resource::kSampler: {
                sampling_manager::thrift::SamplingStrategyResponse response;
                _samplingMgr.getSamplingStrategy(response, serviceName);
                responseJSON = nlohmann::json(response).dump();
            } break;
            default: {
                assert(resource == Resource::kBaggage);
                thrift::BaggageRestrictionManager_getBaggageRestrictions_result
                    response;
                std::vector<thrift::BaggageRestriction> restrictions;
                restrictions.reserve(_restrictions.size());
                std::transform(std::begin(_restrictions),
                               std::end(_restrictions),
                               std::back_inserter(restrictions),
                               [](const KeyRestrictionMap::value_type& pair) {
                                   thrift::BaggageRestriction restriction;
                                   restriction.__set_baggageKey(pair.first);
                                   restriction.__set_maxValueLength(
                                       pair.second.maxValueLength());
                                   return restriction;
                               });
                response.success = std::move(restrictions);
                response.__isset.success = true;
                responseJSON = nlohmann::json(response).dump();
            } break;
            }
            std::ostringstream oss;
            oss << "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n\r\n"
                << responseJSON;
            const auto responseStr = oss.str();
            const auto numWritten = ::send(
                clientSocket.handle(), responseStr.c_str(), responseStr.size(), 0);
            (void)numWritten;
        } catch (const net::http::ParseError& ex) {
            std::ostringstream oss;
            oss << "HTTP/1.1 400 Bad Request\r\n\r\n" << ex.what();
            const auto response = oss.str();
            const auto numWritten = ::send(
                clientSocket.handle(), response.c_str(), response.size(), 0);
            (void)numWritten;
        } catch (const std::exception& ex) {
            std::ostringstream oss;
            oss << "HTTP/1.1 500 Internal Server Error\r\n\r\n" << ex.what();
            const auto response = oss.str();
            const auto numWritten = ::send(
                clientSocket.handle(), response.c_str(), response.size(), 0);
            (void)numWritten;
        }
    }
}

}  // namespace testutils
}  // namespace jaegertracing
