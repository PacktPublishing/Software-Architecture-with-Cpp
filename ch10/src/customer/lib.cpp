#include "customer/customer.h"

#include <cpprest/details/basic_types.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>

using namespace ::web;
using namespace ::web::http;
using ::utility::string_t;

void respond(const http_request &request, status_code status,
             const json::value &response) {
  json::value resp;
  resp[string_t("status")] = json::value::number(status);
  resp[string_t("response")] = response;
  request.reply(status, resp);
}

std::pair<status_code, json::value> prepare_response(const std::string &name) {
  return {status_codes::OK,
          json::value::string(string_t("Hello, ") + name + "!")};
}

void handle_get(const http_request &req) {
  auto parameters = uri::split_query(req.request_uri().query());
  auto keyValueIt = parameters.find(string_t("name"));

  if (keyValueIt == end(parameters)) {
    auto err = string_t("Missing value for 'name'");
    respond(req, status_codes::BadRequest, json::value::string(std::move(err)));
    return;
  }

  const auto &name = keyValueIt->second;
  const auto [code, response] = prepare_response(name);
  respond(req, code, response);
}
