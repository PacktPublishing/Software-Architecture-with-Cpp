#include "customer/customer.h"

#include <gtest/gtest.h>

TEST(basic_responses, given_name_when_prepare_responses_then_greets_friendly) {
  auto name = "Bob";
  auto code_and_string = Responder{}.prepare_response(name);
  ASSERT_EQ(code_and_string.first, web::http::status_codes::OK);
  ASSERT_EQ(code_and_string.second, web::json::value("Hello, Bob!"));
}
