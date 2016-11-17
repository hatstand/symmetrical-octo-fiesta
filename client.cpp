#include <stdio.h>

#include <iostream>
#include <memory>
#include <vector>

#include <gflags/gflags.h>
#include <grpc++/grpc++.h>

#include "service.grpc.pb.h"

using std::cerr;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::to_string;
using std::vector;

DEFINE_int32(port, 8080, "Port to connect to");
DEFINE_string(host, "localhost", "Host to connect to");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  FILE* file = fopen(argv[1], "r");
  fseek(file, 0, SEEK_END);
  int64_t length = ftell(file);
  rewind(file);

  vector<char> data;
  data.resize(length);
  size_t bytes_read =
      fread(data.data(), sizeof(decltype(data)::value_type), length, file);
  if (bytes_read != length) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  cout << "Read image of: " << bytes_read << " bytes" << endl;

  string host = FLAGS_host + ":" + to_string(FLAGS_port);
  cout << "Connecting to: " << host << endl;
  shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel(host, grpc::InsecureChannelCredentials());

  unique_ptr<words::Cheater::Stub> stub = words::Cheater::NewStub(channel);
  words::Request request;
  request.set_image(data.data(), data.size());
  words::Response response;
  grpc::ClientContext context;
  grpc::Status status = stub->FindSolutions(&context, request, &response);

  if (!status.ok()) {
    cerr << status.error_message() << endl;
    return -1;
  }

  cout << response.DebugString() << endl;
  return 0;
}
