#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>

#include <thread>

using namespace ::web;
using namespace ::web::http;
using namespace std::literals;

void respond(const http_request &request, const status_code &status,
             const json::value &response) {
  json::value resp;
  resp[U("status")] = json::value::number(status);
  resp[U("response")] = response;
  request.reply(status, resp);
}

int main() {
  auto listener = ::web::http::experimental::listener::http_listener(
      U("http://localhost/greeter"));
  listener.open().wait();
  listener.support(methods::GET, [](http_request req) {
    auto parameters = uri::split_query(req.request_uri().query());
    auto keyValueIt = parameters.find(U("name"));

    if (keyValueIt == end(parameters)) {
      auto err = U("Missing value for 'name'");
      respond(req, status_codes::BadRequest, json::value::string(err));
      return;
    }

    auto name = keyValueIt->second;
    respond(req, status_codes::OK,
            json::value::string(U("Hello ") + name + "!"));
  });

  std::this_thread::sleep_for(60s);
  listener.close();
}
