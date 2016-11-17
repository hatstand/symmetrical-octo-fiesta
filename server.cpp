#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <gflags/gflags.h>
#include <grpc++/grpc++.h>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "knearest.h"
#include "scrabble.h"
#include "service.grpc.pb.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::unique_ptr;

DEFINE_int32(port, 8080, "Port to start server on");

namespace {

static const int kGridSize = 15;
static const int kRackSize = 7;

char Recognise(const KNearest& nearest, const cv::Mat& image) {
  return nearest.Recognise(image);
}

vector<char> RecogniseGrid(const cv::Mat& image, const KNearest& nearest) {
  cv::Mat grid = image;
  cv::bitwise_not(grid, grid);
  const int grid_start = grid.size().height / 4;
  const int square_width = grid.size().width / kGridSize;

  vector<char> ret(kGridSize * kGridSize);

  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      int x = square_width * i;
      int y = square_width * j + grid_start;

      cv::Point top_left(x, y);
      cv::Point bottom_right(x + square_width, y + square_width);

      cv::Mat square(image, cv::Rect(top_left, bottom_right));
      ret[i + kGridSize * j] = Recognise(nearest, square);
    }
  }

  return ret;
}

vector<char> RecogniseRack(const cv::Mat& image, const KNearest& nearest) {
  const int grid_start = image.size().height / 4;
  const int square_width = image.size().width / kGridSize;
  int fudge = 24;
  int estimate = grid_start + square_width * kGridSize + fudge;
  int guess = estimate + square_width * 2 + fudge;
  const int tablet_width = image.size().width / kRackSize;
  vector<char> ret;
  for (int i = 0; i < kRackSize; ++i) {
    cv::Point top_left(i * tablet_width, estimate);
    cv::Point bottom_right(i * tablet_width + tablet_width, guess);
    cv::Mat square(image, cv::Rect(top_left, bottom_right));
    ret.push_back(Recognise(nearest, square));
  }
  return ret;
}
}

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
    vector<char> grid = RecogniseGrid(image, *knearest_);
    vector<char> rack = RecogniseRack(image, *knearest_);

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
  nearest.Load("model");

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
