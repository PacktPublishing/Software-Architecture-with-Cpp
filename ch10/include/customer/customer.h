#pragma once

#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include <string>
#include <utility>

void handle_get(const ::web::http::http_request& req);

auto prepare_response(const std::string &name)
    -> std::pair<::web::http::status_code, ::web::json::value>;
