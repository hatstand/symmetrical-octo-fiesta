#include "recogniser.h"

using std::vector;

namespace {
static const int kGridSize = 15;
static const int kRackSize = 7;
}  // namespace

Recogniser::Recogniser(const KNearest& nearest) : nearest_(nearest) {}

char Recogniser::Recognise(const cv::Mat& image) const {
  return nearest_.Recognise(image);
}

vector<char> Recogniser::RecogniseGrid(const cv::Mat& image) const {
  const int grid_start = image.size().height / 4;
  const int square_width = image.size().width / kGridSize;

  vector<char> ret(kGridSize * kGridSize);

  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      int x = square_width * i;
      int y = square_width * j + grid_start;

      cv::Point top_left(x, y);
      cv::Point bottom_right(x + square_width, y + square_width);

      cv::Mat square(image, cv::Rect(top_left, bottom_right));
      ret[i + kGridSize * j] = Recognise(square);
    }
  }

  return ret;
  }

  vector<char> Recogniser::RecogniseRack(const cv::Mat& image) const {
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
      ret.push_back(Recognise(square));
    }
    return ret;
  }
