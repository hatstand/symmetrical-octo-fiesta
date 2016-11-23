#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <gflags/gflags.h>
#include <grpc++/grpc++.h>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "knearest.h"
#include "recogniser.h"
#include "scrabble.h"
#include "service.grpc.pb.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::unique_ptr;

DEFINE_int32(port, 8080, "Port to start server on");

class CheaterServiceImpl final : public words::Cheater::Service {
 public:
  CheaterServiceImpl(KNearest* knearest) : knearest_(knearest) {}

  grpc::Status FindSolutions(grpc::ServerContext* context,
                             const words::Request* request,
                             words::Response* response) override {
    const string& data = request->image();
    if (data.empty()) {
      return grpc::Status(grpc::INVALID_ARGUMENT, "wut");
    }
    vector<char> vector_data(data.begin(), data.end());
    cv::Mat image = cv::imdecode(vector_data, 0);
    Recogniser recogniser(*knearest_);
    vector<char> grid = recogniser.RecogniseGrid(image);
    vector<char> rack = recogniser.RecogniseRack(image);

    response->mutable_board()->set_data(grid.data(), grid.size());
    response->mutable_rack()->set_data(rack.data(), rack.size());

    Scrabble scrabble(grid);
    scrabble.PrintBoard();
    vector<Scrabble::Solution> solutions = scrabble.FindBestMove(rack);

    for (const auto& solution : solutions) {
      words::Response::Solution* s = response->add_solution();
      s->set_x(solution.x());
      s->set_y(solution.y());
      s->set_word(solution.word());
      s->set_direction(solution.direction() ==
                               Scrabble::Solution::Direction::ROW
                           ? words::Response::Solution::ROW
                           : words::Response::Solution::COLUMN);
      s->set_score(solution.score());
    }

    return grpc::Status::OK;
  }

 private:
  KNearest* knearest_;
};

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  KNearest nearest;
  nearest.Load("data/model");

  CheaterServiceImpl service(&nearest);

  grpc::ServerBuilder builder;
  string host = "0.0.0.0:" + to_string(FLAGS_port);
  builder.AddListeningPort(host, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  unique_ptr<grpc::Server> server(builder.BuildAndStart());
  cout << "Listening on: " << host << endl;
  server->Wait();

  return 0;
}
