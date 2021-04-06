#include <cstdlib>
#include <iostream>

#include <redis-cpp/execute.h>
#include <redis-cpp/stream.h>

int main() {
  try {
    auto stream = rediscpp::make_stream("localhost", "6379");

    auto const key = "my_key";

    auto response = rediscpp::execute(*stream, "set", key,
                                      "Some value for 'my_key'", "ex", "60");

    std::cout << "Set key '" << key << "': " << response.as<std::string>()
              << std::endl;

    response = rediscpp::execute(*stream, "get", key);
    std::cout << "Get key '" << key << "': " << response.as<std::string>()
              << std::endl;
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
