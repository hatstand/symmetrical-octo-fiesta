#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

#include <cv.h>
#include <highgui.h>

#include "knearest.h"

static const int kGridSize = 15;

enum SpecialSquare {
  TL = 0,
  DL = 1,
  TW = 2,
  DW = 3,
};

void ShowImage(const cv::Mat& image) {
  cv::imshow("foo", image);
  cv::waitKey(0);
}

void Train(const cv::Mat& image) {
  static int data_counter = 0;

  cv::imshow("foo", image);
  char key = static_cast<char>(cv::waitKey(0));

  if (key == ' ') {
    key = '_';
  }

  std::string directory("training/");
  directory += key;
  mkdir(directory.c_str(), 0777);

  std::cout << directory << std::endl;

  std::string filename(directory + "/" + std::to_string(data_counter++) +
                       ".png");

  std::cout << filename << std::endl;

  imwrite(filename, image);
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
    std::cout << file_path << std::endl;
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

void Recognise(KNearest* nearest, const cv::Mat& image) {
  char recognised = nearest->Recognise(image);
  std::cout << "Recognised as: " << recognised << std::endl;
  cv::imshow("foo", image);
  cv::waitKey(0);
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

int main(int argc, char** argv) {
  KNearest* nearest = Foo();
  cv::Mat words = cv::imread("words.png", 0);
  cv::bitwise_not(words, words);

  int grid_start = words.size().height / 4;

  cv::line(words, cv::Point(0, grid_start),
           cv::Point(words.size().width, grid_start), CV_RGB(0, 0, 128));

  for (int i = 0; i < kGridSize; ++i) {
    int x = (words.size().width / 15) * i;
    cv::line(words, cv::Point(x, 0), cv::Point(x, words.size().height),
             CV_RGB(0, 0, 128));

    cv::line(words, cv::Point(0, x + grid_start),
             cv::Point(words.size().width, x + grid_start), CV_RGB(0, 0, 128));
  }

  const int square_width = words.size().width / kGridSize;

  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      int x = square_width * i;
      int y = square_width * j + grid_start;

      cv::Point top_left(x, y);
      cv::Point bottom_right(x + square_width, y + square_width);

      cv::Mat square(words, cv::Rect(top_left, bottom_right));
      // Train(square);
      Recognise(nearest, square);
    }
  }

  ShowImage(words);

  return 0;
}
