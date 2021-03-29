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

#include "Server.h"

#include <atomic>
#include <cstdlib>
#include <future>
#include <sstream>
#include <thread>

#include <nlohmann/json.hpp>

#include "jaegertracing/Tracer.h"
#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/net/Socket.h"
#include "jaegertracing/net/http/Request.h"
#include "jaegertracing/net/http/Response.h"

namespace jaegertracing {
namespace crossdock {
namespace thrift {

#define JSON_FROM_FIELD(var, field)                                            \
    {                                                                          \
        json[#field] = var.field;                                              \
    }

#define FIELD_FROM_JSON(var, field)                                            \
    {                                                                          \
        var.__set_##field(json.at(#field));                                    \
    }

void to_json(nlohmann::json& json, const Transport::type& transport)
{
    json = _Transport_VALUES_TO_NAMES.at(static_cast<int>(transport));
}

void from_json(const nlohmann::json& json, Transport::type& transport)
{
    const auto str = json.get<std::string>();
    if (str == "HTTP") {
        transport = Transport::HTTP;
        return;
    }
    if (str == "TCHANNEL") {
        transport = Transport::TCHANNEL;
        return;
    }
    if (str == "DUMMY") {
        transport = Transport::DUMMY;
        return;
    }
    std::ostringstream oss;
    oss << "Invalid transport value " << str;
    throw std::invalid_argument(oss.str());
}

void to_json(nlohmann::json& json, const Downstream& downstream)
{
    JSON_FROM_FIELD(downstream, serviceName);
    JSON_FROM_FIELD(downstream, serverRole);
    JSON_FROM_FIELD(downstream, host);
    JSON_FROM_FIELD(downstream, port);
    JSON_FROM_FIELD(downstream, transport);
    if (downstream.downstream) {
        json["downstream"] = *downstream.downstream;
    }
}

void from_json(const nlohmann::json& json, Downstream& downstream)
{
    FIELD_FROM_JSON(downstream, serviceName);
    FIELD_FROM_JSON(downstream, serverRole);
    FIELD_FROM_JSON(downstream, host);
    FIELD_FROM_JSON(downstream, port);
    downstream.__set_transport(json.at("transport").get<Transport::type>());
    auto itr = json.find("downstream");
    if (itr != std::end(json) && !itr->is_null()) {
        downstream.__set_downstream(
            std::make_shared<Downstream>(itr->get<Downstream>()));
    }
}

void to_json(nlohmann::json& json, const StartTraceRequest& request)
{
    JSON_FROM_FIELD(request, serverRole);
    JSON_FROM_FIELD(request, sampled);
    JSON_FROM_FIELD(request, baggage);
    JSON_FROM_FIELD(request, downstream);
}

void from_json(const nlohmann::json& json, StartTraceRequest& request)
{
    FIELD_FROM_JSON(request, serverRole);
    FIELD_FROM_JSON(request, sampled);
    FIELD_FROM_JSON(request, baggage);
    FIELD_FROM_JSON(request, downstream);
}

void to_json(nlohmann::json& json, const JoinTraceRequest& request)
{
    JSON_FROM_FIELD(request, serverRole);
    if (request.__isset.downstream) {
        json["downstream"] = request.downstream;
    }
}

void from_json(const nlohmann::json& json, JoinTraceRequest& request)
{
    FIELD_FROM_JSON(request, serverRole);
    auto itr = json.find("downstream");
    if (itr != std::end(json) && !itr->is_null()) {
        request.__set_downstream(itr->get<Downstream>());
    }
}

void to_json(nlohmann::json& json, const ObservedSpan& observedSpan)
{
    JSON_FROM_FIELD(observedSpan, traceId);
    JSON_FROM_FIELD(observedSpan, sampled);
    JSON_FROM_FIELD(observedSpan, baggage);
}

void from_json(const nlohmann::json& json, ObservedSpan& observedSpan)
{
    FIELD_FROM_JSON(observedSpan, traceId);
    FIELD_FROM_JSON(observedSpan, sampled);
    FIELD_FROM_JSON(observedSpan, baggage);
}

void to_json(nlohmann::json& json, const TraceResponse& response)
{
    if (response.__isset.span) {
        JSON_FROM_FIELD(response, span);
    }
    if (response.downstream) {
        json["downstream"] = *response.downstream;
    }
    JSON_FROM_FIELD(response, notImplementedError);
}

void from_json(const nlohmann::json& json, TraceResponse& response)
{
    auto itr = json.find("span");
    if (itr != std::end(json) && !itr->is_null()) {
        response.__set_span(itr->get<ObservedSpan>());
    }
    itr = json.find("downstream");
    if (itr != std::end(json) && !itr->is_null()) {
        response.__set_downstream(
            std::make_shared<TraceResponse>(itr->get<TraceResponse>()));
    }
    FIELD_FROM_JSON(response, notImplementedError);
}

#undef FIELD_FROM_JSON
#undef JSON_FROM_FIELD

}  // namespace thrift

namespace {

constexpr auto kBaggageKey = "crossdock-baggage-key";
constexpr auto kDefaultTracerServiceName = "crossdock-cpp";

std::string escape(const std::string& str)
{
    std::string result;
    result.reserve(str.size());
    for (auto&& ch : str) {
        switch (ch) {
        case '\n': {
            result += "\\n";
        } break;
        case '\r': {
            result += "\\r";
        } break;
        default: {
            result += ch;
        } break;
        }
    }
    return result;
}

std::string bufferedRead(net::Socket& socket)
{
    constexpr auto kBufferSize = 256;
    std::array<char, kBufferSize> buffer;
    std::string data;
    auto numRead = ::read(socket.handle(), &buffer[0], buffer.size());
    data.append(&buffer[0], numRead);
    while (numRead == kBufferSize) {
        numRead = ::read(socket.handle(), &buffer[0], buffer.size());
        data.append(&buffer[0], numRead);
    }
    return data;
}

class RequestReader : public opentracing::HTTPHeadersReader {
  public:
    explicit RequestReader(const net::http::Request& request)
        : _request(request)
    {
    }

    opentracing::expected<void> ForeachKey(
        std::function<opentracing::expected<void>(opentracing::string_view,
                                                  opentracing::string_view)> f)
        const override
    {
        for (auto&& header : _request.headers()) {
            const auto result = f(header.key(), header.value());
            if (!result) {
                return result;
            }
        }
        return opentracing::make_expected();
    }

  private:
    const net::http::Request& _request;
};

class RequestWriter : public opentracing::HTTPHeadersWriter {
  public:
    explicit RequestWriter(std::ostream& requestStream)
        : _requestStream(requestStream)
    {
    }

    opentracing::expected<void>
    Set(opentracing::string_view key,
        opentracing::string_view value) const override
    {
        _requestStream << key << ": " << value << "\r\n";
        return opentracing::make_expected();
    }

  private:
    std::ostream& _requestStream;
};

thrift::ObservedSpan observeSpan(const opentracing::SpanContext& ctx)
{
    const auto& sc = static_cast<const SpanContext&>(ctx);
    thrift::ObservedSpan observedSpan;
    std::ostringstream oss;
    oss << sc.traceID();
    observedSpan.__set_traceId(oss.str());
    observedSpan.__set_sampled(sc.isSampled());
    auto itr = sc.baggage().find(kBaggageKey);
    if (itr != std::end(sc.baggage())) {
        observedSpan.__set_baggage(itr->second);
    }
    return observedSpan;
}

thrift::TraceResponse callDownstreamHTTP(const opentracing::SpanContext& ctx,
                                         const thrift::Downstream& target,
                                         opentracing::Tracer& tracer,
                                         logging::Logger& logger)
{
    thrift::JoinTraceRequest request;
    request.__set_serverRole(target.serverRole);
    if (target.downstream) {
        request.__set_downstream(*target.downstream);
    }

    const auto requestJSON = nlohmann::json(request).dump();
    net::Socket socket;
    socket.open(AF_INET, SOCK_STREAM);
    const auto authority = target.host + ':' + target.port;
    socket.connect("http://" + authority);
    std::ostringstream oss;
    oss << "POST /join_trace HTTP/1.1\r\n"
           "Host: "
        << authority << "\r\n";
    RequestWriter writer(oss);
    tracer.Inject(ctx, writer);
    oss << "Connection: close\r\n"
           "Content-Type: application/json\r\n"
           "Content-Length: "
        << requestJSON.size() << "\r\n\r\n"
        << requestJSON;
    const auto message = oss.str();
    logger.info("Sending request downstream: " + escape(message));
    const auto numWritten =
        ::write(socket.handle(), &message[0], message.size());
    (void)numWritten;

    const auto responseStr = bufferedRead(socket);
    logger.info("Received downstream response: " + escape(responseStr));
    std::istringstream iss(responseStr);
    auto response = net::http::Response::parse(iss);
    return nlohmann::json::parse(response.body());
}

thrift::TraceResponse callDownstream(const opentracing::SpanContext& ctx,
                                     const std::string& /* role */,
                                     const thrift::Downstream& downstream,
                                     opentracing::Tracer& tracer,
                                     logging::Logger& logger)
{
    thrift::TraceResponse response;

    switch (downstream.transport) {
    case thrift::Transport::HTTP: {
        response = callDownstreamHTTP(ctx, downstream, tracer, logger);
    } break;
    case thrift::Transport::TCHANNEL: {
        response.__set_notImplementedError(
            "TCHANNEL transport not implemented");
    } break;
    case thrift::Transport::DUMMY: {
        response.__set_notImplementedError("DUMMY transport not implemented");
    } break;
    default: {
        throw std::invalid_argument("Unrecognized protocol " +
                                    std::to_string(downstream.transport));
    } break;
    }

    return response;
}

thrift::TraceResponse prepareResponse(const opentracing::SpanContext& ctx,
                                      const std::string& role,
                                      const thrift::Downstream* downstream,
                                      opentracing::Tracer& tracer,
                                      logging::Logger& logger)
{
    const auto observedSpan = observeSpan(ctx);
    thrift::TraceResponse response;
    response.__set_span(observedSpan);
    if (downstream) {
        response.__set_downstream(std::make_shared<thrift::TraceResponse>(
            callDownstream(ctx, role, *downstream, tracer, logger)));
    }
    return response;
}

struct GenerateTracesRequest {
    using StrMap = std::unordered_map<std::string, std::string>;

    std::string _type;
    std::string _operation;
    StrMap _tags;
    int _count;
};

void from_json(const nlohmann::json& json, GenerateTracesRequest& request)
{
    request._type = json.at("type");
    request._operation = json.at("operation");
    request._tags = json.at("tags").get<GenerateTracesRequest::StrMap>();
    request._count = json.at("count");
}

}  // anonymous namespace

using Handler = std::function<std::string(const net::http::Request&)>;

class Server::SocketListener {
  public:
    SocketListener(const net::IPAddress& ip,
                   const std::shared_ptr<logging::Logger>& logger,
                   Handler handler)
        : _ip(ip)
        , _logger(logger)
        , _handler(handler)
        , _running(false)
    {
        assert(_logger);
    }

    ~SocketListener() { stop(); }

    void start()
    {
        std::promise<void> started;
        _thread = std::thread([this, &started]() { start(_ip, started); });
        started.get_future().get();
    }

    void stop() noexcept
    {
        if (_running) {
            _running = false;
            _thread.join();
            _socket.close();
        }
    }

  private:
    void start(const net::IPAddress& ip, std::promise<void>& started)
    {
        _socket.open(AF_INET, SOCK_STREAM);
        const auto enable = 1;
        ::setsockopt(_socket.handle(),
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     &enable,
                     sizeof(enable));
        _socket.bind(ip);
        _socket.listen();
        _running = true;
        started.set_value();

        using TaskList = std::deque<std::future<void>>;
        TaskList tasks;

        while (_running) {
            auto client = _socket.accept();
            auto future = std::async(
                std::launch::async,
                [this](net::Socket&& socket) {
                    net::Socket client(std::move(socket));
                    auto requestStr = bufferedRead(client);
                    _logger->info("Received request: " + escape(requestStr));

                    try {
                        std::istringstream iss(requestStr);
                        const auto request = net::http::Request::parse(iss);
                        const auto responseStr = _handler(request);
                        const auto numWritten = ::write(client.handle(),
                                                        &responseStr[0],
                                                        responseStr.size());
                        if (numWritten !=
                            static_cast<int>(responseStr.size())) {
                            std::ostringstream oss;
                            oss << "Unable to write entire response"
                                   ", numWritten="
                                << numWritten
                                << ", responseSize=" << responseStr.size();
                            _logger->error(oss.str());
                        }
                    } catch (...) {
                        utils::ErrorUtil::logError(*_logger, "Server error");
                        constexpr auto message =
                            "HTTP/1.1 500 Internal Server Error\r\n\r\n";
                        constexpr auto messageSize = sizeof(message) - 1;
                        const auto numWritten =
                            ::write(client.handle(), message, messageSize);
                        (void)numWritten;
                    }

                    client.close();
                },
                std::move(client));
            tasks.emplace_back(std::move(future));
        }

        std::for_each(std::begin(tasks),
                      std::end(tasks),
                      [](TaskList::value_type& future) { future.get(); });
    }

    net::IPAddress _ip;
    net::Socket _socket;
    std::shared_ptr<logging::Logger> _logger;
    Handler _handler;
    std::atomic<bool> _running;
    std::thread _thread;
};

class Server::EndToEndHandler {
  public:
    using TracerPtr = std::shared_ptr<opentracing::Tracer>;

    EndToEndHandler(const std::string& agentHostPort,
                    const std::string& collectorEndpoint,
                    const std::string& samplingServerURL)
        : _agentHostPort(agentHostPort)
        , _collectorEndpoint(collectorEndpoint)
        , _samplingServerURL(samplingServerURL)
    {
    }

    TracerPtr findOrMakeTracer(std::string samplerType)
    {
        if (samplerType.empty()) {
            samplerType = kSamplerTypeRemote;
        }

        std::lock_guard<std::mutex> lock(_mutex);
        auto itr = _tracers.find(samplerType);
        if (itr != std::end(_tracers)) {
            return itr->second;
        }
        return init(samplerType);
    }

  private:
    Config makeEndToEndConfig(const std::string& samplerType) const
    {
        return Config(false,
                      samplers::Config(samplerType,
                                       1.0,
                                       _samplingServerURL,
                                       samplers::Config::kDefaultMaxOperations,
                                       std::chrono::seconds(5)),
                      reporters::Config(reporters::Config::kDefaultQueueSize,
                                        std::chrono::seconds(1),
                                        false,
                                        _agentHostPort,
                                        _collectorEndpoint));
    }

    TracerPtr init(const std::string& samplerType)
    {
        const auto config = makeEndToEndConfig(samplerType);
        auto tracer = Tracer::make(kDefaultTracerServiceName, config);
        _tracers[config.sampler().type()] = tracer;
        return tracer;
    }

    std::string _agentHostPort;
    std::string _collectorEndpoint;
    std::string _samplingServerURL;
    std::unordered_map<std::string, TracerPtr> _tracers;
    std::mutex _mutex;
};

Server::Server(const net::IPAddress& clientIP,
               const net::IPAddress& serverIP,
               const std::string& agentHostPort,
               const std::string& collectorEndpoint,
               const std::string& samplingServerURL)
    : _logger(logging::consoleLogger())
    , _tracer(Tracer::make(kDefaultTracerServiceName, Config(), _logger))
    , _clientListener(
          new SocketListener(clientIP,
                             _logger,
                             [this](const net::http::Request& request) {
                                 return handleRequest(request);
                             }))
    , _serverListener(
          new SocketListener(serverIP,
                             _logger,
                             [this](const net::http::Request& request) {
                                 return handleRequest(request);
                             }))
    , _handler(new EndToEndHandler(agentHostPort, collectorEndpoint, samplingServerURL))
{
}

Server::~Server() = default;

void Server::serve()
{
    _clientListener->start();
    _serverListener->start();
}

template <typename RequestType>
std::string Server::handleJSON(
    const net::http::Request& request,
    std::function<thrift::TraceResponse(
        const RequestType&, const opentracing::SpanContext&)> handler)
{
    RequestReader reader(request);
    auto result = _tracer->Extract(reader);
    if (!result) {
        std::ostringstream oss;
        oss << "Cannot read request body: opentracing error code "
            << result.error().value();
        const auto message = oss.str();
        oss.str("");
        oss.clear();
        oss << "HTTP/1.1 400 Bad Request\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
    }

    std::unique_ptr<opentracing::SpanContext> ctx(result->release());
    opentracing::StartSpanOptions options;
    options.start_system_timestamp = std::chrono::system_clock::now();
    options.start_steady_timestamp = std::chrono::steady_clock::now();
    if (ctx) {
        options.references.emplace_back(std::make_pair(
            opentracing::SpanReferenceType::ChildOfRef, ctx.get()));
    }
    auto span = _tracer->StartSpanWithOptions("post", options);

    RequestType thriftRequest;
    try {
        thriftRequest = nlohmann::json::parse(request.body());
    } catch (const std::exception& ex) {
        std::ostringstream oss;
        oss << "Cannot parse request JSON: " << ex.what()
            << ", json: " << request.body();
        const auto message = oss.str();
        oss.str("");
        oss.clear();
        oss << "HTTP/1.1 500 Internal Server Error\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
        return oss.str();
    } catch (...) {
        std::ostringstream oss;
        oss << "Cannot parse request JSON, json: " << request.body();
        const auto message = oss.str();
        oss.str("");
        oss.clear();
        oss << "HTTP/1.1 500 Internal Server Error\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
        return oss.str();
    }

    const auto thriftResponse = handler(thriftRequest, span->context());
    try {
        const auto message = nlohmann::json(thriftResponse).dump();
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
        return oss.str();
    } catch (const std::exception& ex) {
        std::ostringstream oss;
        oss << "Cannot marshal response to JSON: " << ex.what();
        const auto message = oss.str();
        oss.str("");
        oss.clear();
        oss << "HTTP/1.1 500 Internal Server Error\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
        return oss.str();
    } catch (...) {
        std::ostringstream oss;
        oss << "Cannot marshal response to JSON";
        const auto message = oss.str();
        oss.str("");
        oss.clear();
        oss << "HTTP/1.1 500 Internal Server Error\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
        return oss.str();
    }
}

std::string Server::handleRequest(const net::http::Request& request)
{
    if (request.target() == "/") {
        return "HTTP/1.1 200 OK\r\n\r\n";
    }
    if (request.target() == "/start_trace") {
        return handleJSON<thrift::StartTraceRequest>(
            request,
            [this](const thrift::StartTraceRequest& request,
                   const opentracing::SpanContext& /* ctx */) {
                return startTrace(request);
            });
    }
    if (request.target() == "/join_trace") {
        return handleJSON<thrift::JoinTraceRequest>(
            request,
            [this](const thrift::JoinTraceRequest& request,
                   const opentracing::SpanContext& ctx) {
                return joinTrace(request, ctx);
            });
    }
    if (request.target() == "/create_traces") {
        return generateTraces(request);
    }
    return "HTTP/1.1 404 Not Found\r\n\r\n";
}

thrift::TraceResponse
Server::startTrace(const crossdock::thrift::StartTraceRequest& request)
{
    auto span = _tracer->StartSpan(request.serverRole);
    if (request.sampled) {
        span->SetTag("sampling.priority", 1);
    }
    span->SetBaggageItem(kBaggageKey, request.baggage);

    return prepareResponse(span->context(),
                           request.serverRole,
                           &request.downstream,
                           *_tracer,
                           *_logger);
}

thrift::TraceResponse
Server::joinTrace(const crossdock::thrift::JoinTraceRequest& request,
                  const opentracing::SpanContext& ctx)
{
    return prepareResponse(ctx,
                           request.serverRole,
                           request.__isset.downstream ? &request.downstream
                                                      : nullptr,
                           *_tracer,
                           *_logger);
}

std::string Server::generateTraces(const net::http::Request& requestHTTP)
{
    GenerateTracesRequest request;
    try {
        request = nlohmann::json::parse(requestHTTP.body());
    } catch (const std::exception& ex) {
        std::ostringstream oss;
        oss << "JSON payload is invalid: " << ex.what();
        const auto message = oss.str();
        oss.str("");
        oss.clear();
        oss << "HTTP/1.1 400 Bad Request\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
        return oss.str();
    } catch (...) {
        const std::string message("JSON payload is invalid");
        std::ostringstream oss;
        oss << "HTTP/1.1 400 Bad Request\r\n"
               "Content-Length: "
            << message.size() << "\r\n\r\n"
            << message;
        return oss.str();
    }

    auto tracer = _handler->findOrMakeTracer(request._type);
    if (!tracer) {
        const std::string message("Tracer is not initialized");
        std::ostringstream oss;
        oss << "HTTP/1.1 500 Internal Server Error\r\n"
               "Content-Length: "
            << message.size() << "\r\n"
            << message;
        return oss.str();
    }

    for (auto i = 0; i < request._count; ++i) {
        auto span = tracer->StartSpan(request._operation);
        for (auto&& pair : request._tags) {
            span->SetTag(pair.first, pair.second);
        }
        span->Finish();
    }

    return "HTTP/1.1 200 OK\r\n\r\n";
}

}  // namespace crossdock
}  // namespace jaegertracing

int main()
{
    const auto rawSenderType = std::getenv("SENDER");
    const std::string senderType(rawSenderType ? rawSenderType : "");

    if (senderType.empty()) {
      std::cerr << "env SENDER is not specified!\n";
      return 1;
    }

    const auto rawAgentHostPort = std::getenv("AGENT_HOST_PORT");
    const std::string agentHostPort(rawAgentHostPort ? rawAgentHostPort : "");

    if (agentHostPort.empty() && senderType == "udp") {
      std::cerr << "env AGENT_HOST_PORT is not specified!\n";
      return 1;
    }

    const std::string collectorEndpoint(senderType == "http" ? "http://jaeger-collector:14268/api/traces" : "");

    const auto rawSamplingServerURL = std::getenv("SAMPLING_SERVER_URL");
    const std::string samplingServerURL(
        rawSamplingServerURL ? rawSamplingServerURL : "");
    if (samplingServerURL.empty()) {
        std::cerr << "env SAMPLING_SERVER_URL is not specified!\n";
        return 1;
    }

    jaegertracing::crossdock::Server server(
        jaegertracing::net::IPAddress::v4("0.0.0.0:8080"),
        jaegertracing::net::IPAddress::v4("0.0.0.0:8081"),
        agentHostPort,
        collectorEndpoint,
        samplingServerURL);
    server.serve();

    std::this_thread::sleep_for(std::chrono::hours(1));
    return 0;
}
