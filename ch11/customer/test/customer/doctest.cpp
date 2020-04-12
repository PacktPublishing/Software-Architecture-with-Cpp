#include "customer/customer.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("Basic responses") {
  auto name = "Bob";
  auto code_and_string = responder{}.prepare_response(name);
  REQUIRE(code_and_string.first == web::http::status_codes::OK);
  REQUIRE(code_and_string.second == web::json::value("Hello, Bob!"));
}
