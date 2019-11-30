#include "customer/customer.h"

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>

#include <thread>

using namespace ::web;
using namespace ::web::http;
using namespace std::literals;
using ::utility::string_t;

int main() {
  using ::web::http::experimental::listener::http_listener;
  auto listener = http_listener{string_t("http://0.0.0.0:8080/customer")};

  listener.support(methods::GET, handle_get); // other verbs coming soon!

  listener.open().wait();

  std::this_thread::sleep_for(60s);
  listener.close().wait();
}
