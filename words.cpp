#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

#include <cv.h>
#include <highgui.h>

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
      Train(square);
    }
  }

  ShowImage(words);

  return 0;
}
