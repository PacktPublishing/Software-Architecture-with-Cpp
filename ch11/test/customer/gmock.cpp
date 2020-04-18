#include "customer/customer.h"

#include <gmock/gmock.h>

using namespace ::testing;
using namespace ::web;
using namespace ::web::http;

namespace {

class ResponderMock {
 public:
  MOCK_METHOD((std::pair<::web::http::status_code,
                         ::web::json::value>),  // note the parens
              prepare_response, (const std::string &name), ());
  MOCK_METHOD(void, respond,
              (const ::web::http::http_request &request,
               ::web::http::status_code status,
               const ::web::json::value &response),
              ());
};

MATCHER_P(ContainsString, string, "") { return arg.as_string() == string; }

}  // namespace

TEST(basic_responses,
     given_name_when_handle_get_then_response_is_prepared_and_sent) {
  http_request request{"GET"};
  request.set_request_uri("/customer?name=Bob");

  auto responder = StrictMock<ResponderMock>{};
  auto response = json::value{"my response"};
  EXPECT_CALL(responder, prepare_response("Bob"))
      .WillOnce(Return(std::pair{status_codes::OK, response}));
  EXPECT_CALL(responder, respond(Ref(request), status_codes::OK,
                                 ContainsString("my response")));
  handle_get(request, responder);
}
