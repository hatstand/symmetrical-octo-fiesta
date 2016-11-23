#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "knearest.h"
#include "scrabble.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;

namespace {

static const int kGridSize = 15;
static const int kRackSize = 7;

void ShowImage(const cv::Mat& image) {
  cv::imshow("foo", image);
  cv::waitKey(0);
}

int GetNext(const string& path) {
  DIR* dir = opendir(path.c_str());

  int next = 0;

  dirent* directory_info = nullptr;
  while ((directory_info = readdir(dir))) {
    string name(directory_info->d_name);
    string number;
    for (char c : name) {
      if (c >= '0' && c <= '9') {
        number += c;
      } else {
        break;
      }
    }
    if (!number.empty()) {
      int x = atoi(number.c_str());
      if (x > next) {
        next = x;
      }
    }
  }
  closedir(dir);
  return next + 1;
}

char Train(const cv::Mat& image) {
  cv::imshow("foo", image);
  char key = static_cast<char>(cv::waitKey(0));

  if (key == ' ') {
    key = '_';
  }

  string directory("training/");
  directory += key;
  mkdir(directory.c_str(), 0777);

  string filename(directory + "/" + to_string(GetNext(directory)) + ".png");

  imwrite(filename, image);
  return key;
}

char Recognise(KNearest* nearest, const cv::Mat& image) {
  char recognised = nearest->Recognise(image);
  if (recognised == '?') {
    // Didn't manage this one. Ask a human.
    return Train(image);
  }
  return recognised;
}

vector<char> RecogniseRack(const cv::Mat& image, KNearest* nearest) {
  const int grid_start = image.size().height / 4;
  const int square_width = image.size().width / kGridSize;
  int fudge = 24;
  int estimate = grid_start + square_width * kGridSize + fudge;
  int guess = estimate + square_width * 2 + fudge;
  const int tablet_width = image.size().width / 7;
  vector<char> ret;
  for (int i = 0; i < kRackSize; ++i) {
    cv::Point top_left(i * tablet_width, estimate);
    cv::Point bottom_right(i * tablet_width + tablet_width, guess);

    cv::Mat square(image, cv::Rect(top_left, bottom_right));
    ret.push_back(Recognise(nearest, square));
  }
  return ret;
}

void RecogniseGrid(const string& path, KNearest* nearest) {
  cv::Mat image = cv::imread(path, 0);
  cv::bitwise_not(image, image);
  const int grid_start = image.size().height / 4;
  const int square_width = image.size().width / kGridSize;

  vector<char> grid(kGridSize * kGridSize);

  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      int x = square_width * i;
      int y = square_width * j + grid_start;

      cv::Point top_left(x, y);
      cv::Point bottom_right(x + square_width, y + square_width);

      cv::Mat square(image, cv::Rect(top_left, bottom_right));
      grid[i + kGridSize * j] = Recognise(nearest, square);
    }
  }

  vector<char> rack = RecogniseRack(image, nearest);
  cout << "RACK:" << endl;
  for (char c : rack) {
    cout << c << " ";
  }
  cout << endl;

  Scrabble scrabble(grid);
  scrabble.PrintBoard();
  scrabble.FindBestMove(rack);
}
}  // namespace

int main(int argc, char** argv) {
  KNearest nearest;
  nearest.Load("data/model");
  RecogniseGrid(argv[1], &nearest);
  return 0;
}
