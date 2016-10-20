#ifndef KNEAREST_H
#define KNEAREST_H

#include <memory>

#include <opencv2/ml/ml.hpp>

class KNearest {
 public:
  void Learn(const cv::Mat& image, char c);
  void Train();
  char Recognise(const cv::Mat& image);

 private:
  cv::Mat PrepareSample(const cv::Mat& image);

  cv::Mat samples_;
  cv::Mat responses_;

  std::unique_ptr<cv::KNearest> model_;
};

#endif  // KNEAREST_H
