#include <cpprest/filestream.h>
#include <cpprest/http_client.h>

#include <fstream>
#include <memory>
#include <ostream>

using namespace concurrency::streams;
using namespace web;
using namespace web::http;
using namespace web::http::client;

int main() {
  auto fileStream = std::make_unique<ostream>();

  pplx::task<void> httpRequestTask =
      fstream::open_ostream(U("response.html"))
          .then([&fileStream](ostream fout) {
            *fileStream = fout;

            auto client = http_client(U("http://localhost:3000"));
            auto builder = uri_builder(U("/greeter"));
            builder.append_query(U("name"), U("World"));
            return client.request(methods::GET, builder.to_string());
          })
          .then([&fileStream](http_response response) {
            std::cout << "response:" << std::endl
                      << response.to_string() << std::endl;

            // write the response to file
            response.body().read_to_end(fileStream->streambuf());
          });
  try {
    httpRequestTask.wait();
  } catch (const std::exception &e) {
    // handle the exception
    std::cerr << e.what();
  }
}
