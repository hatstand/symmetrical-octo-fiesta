#include <memory>

#include <grpc++/grpc++.h>

#include "service.grpc.pb.h"

using std::unique_ptr;

class CheaterServiceImpl final : public words::Cheater::Service {
  grpc::Status FindSolutions(grpc::ServerContext* context,
                             const words::Request* request,
                             words::Response* response) override {
    return grpc::Status::OK;
  }
};

int main(int argc, char** argv) {
  CheaterServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:8080", grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();

  return 0;
}
