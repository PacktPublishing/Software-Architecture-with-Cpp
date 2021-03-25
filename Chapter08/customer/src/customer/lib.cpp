#include <cpprest/details/basic_types.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>

#include "customer/customer.h"

using namespace ::web;
using namespace ::web::http;
using ::utility::string_t;

auto responder::respond(const http_request &request, status_code status,
                        const json::value &response) -> void {
  json::value resp;
  resp[string_t("status")] = json::value::number(status);
  resp[string_t("response")] = response;
  request.reply(status, resp);
}

auto responder::prepare_response(const std::string &name)
    -> std::pair<status_code, json::value> {
  return {status_codes::OK,
          json::value::string(string_t("Hello, ") + name + "!")};
}
