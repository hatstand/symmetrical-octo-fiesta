#ifndef KNEAREST_H
#define KNEAREST_H

#include <memory>

#include "opencv2/ml/ml.hpp"

class KNearest {
 public:
  KNearest();
  void Learn(const cv::Mat& image, char c);
  void Train();
  char Recognise(const cv::Mat& image) const;
  void Save(const std::string& path);
  void Load(const std::string& path);

 private:
  cv::Mat PrepareSample(const cv::Mat& image) const;

  cv::Mat samples_;
  cv::Mat responses_;

  cv::Ptr<cv::ml::KNearest> model_;
};

#endif  // KNEAREST_H
