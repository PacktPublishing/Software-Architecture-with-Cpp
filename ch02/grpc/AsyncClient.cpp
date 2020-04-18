#include <grpcpp/grpcpp.h>
#include <string>
#include "greeter.pb.h"

int main() {
  std::string address("localhost");
    auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials();
    auto stub = Greeter::NewStub(channel);

    GreetRequest request;
    request.set_name("World");

    GreetReply reply;
    ClientContext context;
    Status status = stub->sendRequest(&context, request, &reply);

    if (status.ok()) {
    std::cout << reply.result() << '\n';
    } else {
    std::cerr << "Error: " << status.error_code() << '\n';
    }
}
