#include <vector>

#include "opencv2/imgcodecs/imgcodecs.hpp"

#include "knearest.h"
#include "recogniser.h"
#include "scrabble.h"

using std::vector;

int main(int argc, char** argv) {
  KNearest nearest;
  nearest.Load("data/model");

  Recogniser recogniser(nearest);
  cv::Mat image = cv::imread(argv[1], 0);
  cv::bitwise_not(image, image);
  vector<char> grid = recogniser.RecogniseGrid(image);
  vector<char> rack = recogniser.RecogniseRack(image);

  Scrabble scrabble(grid);
  scrabble.PrintBoard();
  scrabble.FindBestMove(rack);

  return 0;
}
