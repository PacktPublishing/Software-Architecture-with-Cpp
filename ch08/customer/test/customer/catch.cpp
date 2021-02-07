#include "customer/customer.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
                           // this in one cpp file
#include "catch2/catch.hpp"

TEST_CASE("Basic responses",
          "Given Name When Prepare Responses Then Greets Friendly") {
  auto name = "Bob";
  auto code_and_string = responder{}.prepare_response(name);
  REQUIRE(code_and_string.first == web::http::status_codes::OK);
  REQUIRE(code_and_string.second == web::json::value("Hello, Bob!"));
}
