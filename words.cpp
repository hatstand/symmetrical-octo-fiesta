#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "knearest.h"
#include "scrabble.h"

static const int kGridSize = 15;
static const int kTabletSize = 7;

void ShowImage(const cv::Mat& image) {
  cv::imshow("foo", image);
  cv::waitKey(0);
}

int GetNext(const std::string& path) {
  DIR* dir = opendir(path.c_str());

  int next = 0;

  dirent* directory_info = nullptr;
  while ((directory_info = readdir(dir))) {
    std::string name(directory_info->d_name);
    std::string number;
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

  std::string directory("training/");
  directory += key;
  mkdir(directory.c_str(), 0777);

  std::string filename(directory + "/" + std::to_string(GetNext(directory)) +
                       ".png");

  imwrite(filename, image);
  return key;
}

void TrainDirectory(const std::string path, const std::string name,
                    KNearest* knearest) {
  DIR* training_directory = opendir(path.c_str());

  dirent* directory_info = nullptr;
  while ((directory_info = readdir(training_directory))) {
    if (directory_info->d_type != DT_REG) {
      continue;
    }
    std::string file_path = path + "/";
    file_path += directory_info->d_name;
    cv::Mat image = cv::imread(file_path, 0);
    knearest->Learn(image, *path.rbegin());
  }
  closedir(training_directory);
}

KNearest* Foo() {
  DIR* training_directory = opendir("training");

  dirent* directory_info = nullptr;
  KNearest* knearest = new KNearest;
  while ((directory_info = readdir(training_directory))) {
    if (directory_info->d_type != DT_DIR) {
      continue;
    }
    if (strcmp(directory_info->d_name, ".") == 0 ||
        strcmp(directory_info->d_name, "..") == 0) {
      continue;
    }

    std::string path("training/");
    path += directory_info->d_name;

    TrainDirectory(path, directory_info->d_name, knearest);
  }
  closedir(training_directory);

  knearest->Train();
  return knearest;
}

char Recognise(KNearest* nearest, const cv::Mat& image) {
  char recognised = nearest->Recognise(image);
  if (recognised == '?') {
    // Didn't manage this one. Ask a human.
    return Train(image);
  }
  return recognised;
}

void DrawLine(const cv::Vec2f& line, cv::Mat* image, cv::Scalar rgb) {
  if (line[1] != 0) {
    float m = -1 / tan(line[1]);
    float c = line[0] / sin(line[1]);
    cv::line(*image, cv::Point(0, c),
             cv::Point(image->size().width, m * image->size().width + c), rgb);
  } else {
    cv::line(*image, cv::Point(line[0], 0),
             cv::Point(line[0], image->size().height), rgb);
  }
}

std::vector<char> RecogniseTablet(const cv::Mat& image, KNearest* nearest) {
  const int grid_start = image.size().height / 4;
  const int square_width = image.size().width / kGridSize;
  int fudge = 24;
  int estimate = grid_start + square_width * kGridSize + fudge;
  int guess = estimate + square_width * 2 + fudge;
  const int tablet_width = image.size().width / 7;
  std::vector<char> ret;
  for (int i = 0; i < kTabletSize; ++i) {
    cv::Point top_left(i * tablet_width, estimate);
    cv::Point bottom_right(i * tablet_width + tablet_width, guess);

    cv::Mat square(image, cv::Rect(top_left, bottom_right));
    ret.push_back(Recognise(nearest, square));
  }
  return ret;
}

void RecogniseGrid(const std::string& path, KNearest* nearest) {
  cv::Mat image = cv::imread(path, 0);
  cv::bitwise_not(image, image);
  const int grid_start = image.size().height / 4;
  const int square_width = image.size().width / kGridSize;

  char* grid = static_cast<char*>(malloc(kGridSize * kGridSize));

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

  std::vector<char> tablet = RecogniseTablet(image, nearest);
  std::cout << "TABLET:" << std::endl;
  for (char c : tablet) {
    std::cout << c << " ";
  }
  std::cout << std::endl;

  Scrabble scrabble(grid);
  scrabble.PrintBoard();
  scrabble.FindBestMove(tablet);
  scrabble.PrintBoard();
  free(grid);
}

int main(int argc, char** argv) {
  KNearest* nearest = Foo();
  RecogniseGrid(argv[1], nearest);
  return 0;
}
