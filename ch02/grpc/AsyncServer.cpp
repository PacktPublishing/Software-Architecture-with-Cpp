#include <grpcpp/grpcpp.h>
#include <string>
#include "greeter.pb.h"

class Greeter : public Greeter::Service {
  Status sendRequest(ServerContext *context, const GreetRequest *request,
                     GreetReply *reply) override {
    auto name = request->name();
    if (name.empty()) {
      return Status::INVALID_ARGUMENT;
    }
    reply->set_result("Hello " + name);
    return Status::OK;
  }
};

int main() {
  std::string address("localhost");
  Greeter service;
  ServerBuilder builder;

  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  auto server(builder.BuildAndStart());
  server->Wait();
}
