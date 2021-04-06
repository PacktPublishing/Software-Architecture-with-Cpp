#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <jaegertracing/Tracer.h>

#include <thread>

#include "customer/customer.h"

using namespace ::web;
using namespace ::web::http;
using namespace std::literals;
using ::utility::string_t;

void setUpTracer() {
  // We want to read the sampling server configuration from the
  // environment variables
  auto config = jaegertracing::Config();
  config.fromEnv();
  // Jaeger provides us with ConsoleLogger and NullLogger
  auto tracer = jaegertracing::Tracer::make(
      "customer", config, jaegertracing::logging::consoleLogger());
  opentracing::Tracer::InitGlobal(
      std::static_pointer_cast<opentracing::Tracer>(tracer));
}

int main() {
  using ::web::http::experimental::listener::http_listener;
  auto listener = http_listener{string_t("http://0.0.0.0:8080/customer")};

  setUpTracer();

  auto get_responder = responder{};
  listener.support(
      methods::GET,  // other verbs coming soon!
      [&](const auto &request) { handle_get(request, get_responder); });

  listener.open().wait();

  std::this_thread::sleep_for(60s);
  listener.close().wait();
}
