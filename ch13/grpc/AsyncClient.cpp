#include <grpcpp/grpcpp.h>

#include <string>

#include "grpc/service.grpc.pb.h"

using grpc::ClientContext;
using grpc::Status;

int main() {
  std::string address("localhost:50000");
  auto channel =
      grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
  auto stub = Greeter::NewStub(channel);

  GreetRequest request;
  request.set_name("World");

  GreetResponse reply;
  ClientContext context;
  Status status = stub->Greet(&context, request, &reply);

  if (status.ok()) {
    std::cout << reply.reply() << '\n';
  } else {
    std::cerr << "Error: " << status.error_code() << '\n';
  }
}
