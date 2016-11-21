#ifndef RECOGNISER_H
#define RECOGNISER_H

#include <vector>

#include <opencv2/core/core.hpp>

#include "knearest.h"

class Recogniser {
 public:
  explicit Recogniser(const KNearest& nearest);

  char Recognise(const cv::Mat& image) const;
  std::vector<char> RecogniseGrid(const cv::Mat& image) const;
  std::vector<char> RecogniseRack(const cv::Mat& image) const;

 private:
  const KNearest& nearest_;
};

#endif  // RECOGNISER_H
