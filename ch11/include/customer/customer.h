#pragma once

#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include <string>
#include <utility>

class Responder {
 public:
  auto prepare_response(const std::string &name)
      -> std::pair<::web::http::status_code, ::web::json::value>;

  auto respond(const ::web::http::http_request &request,
               ::web::http::status_code status,
               const ::web::json::value &response) -> void;
};

template <typename Responder>
void handle_get(const ::web::http::http_request &req, Responder &responder) {
  using namespace ::web;
  using namespace ::web::http;
  using namespace ::utility;

  auto parameters = uri::split_query(req.request_uri().query());
  auto keyValueIt = parameters.find(string_t("name"));

  if (keyValueIt == end(parameters)) {
    auto err = string_t("Missing value for 'name'");
    responder.respond(req, status_codes::BadRequest,
                      json::value::string(std::move(err)));
    return;
  }

  const auto &name = keyValueIt->second;
  const auto [code, response] = responder.prepare_response(name);
  responder.respond(req, code, response);
}
