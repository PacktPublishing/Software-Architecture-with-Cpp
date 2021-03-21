#include <grpcpp/grpcpp.h>

#include <string>

#include "grpc/service.grpc.pb.h"

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;

class GreeterImpl : public Greeter::Service {
  Status Greet(ServerContext *context, const GreetRequest *request,
               GreetResponse *reply) override {
    auto name = request->name();
    if (name.empty()) {
      return Status(StatusCode::INVALID_ARGUMENT, "name is empty");
    }
    reply->set_reply("Hello " + name);
    return Status::OK;
  }
};

int main() {
  std::string address("localhost:50000");
  GreeterImpl service;
  ServerBuilder builder;

  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  auto server(builder.BuildAndStart());
  server->Wait();
}
