#ifndef KNEAREST_H
#define KNEAREST_H

#include <functional>
#include <memory>

#include "opencv2/ml/ml.hpp"

class KNearest {
 public:
  KNearest(std::function<char(const cv::Mat&)> recognise_callback = nullptr);
  void Learn(const cv::Mat& image, char c);
  void Train();
  // Returns the closest match to an actual tile.
  char Recognise(const cv::Mat& image) const;
  void Save(const std::string& path);
  bool Load(const std::string& path);
  bool LoadFromString(const std::string& model);

 private:
  cv::Mat PrepareSample(const cv::Mat& image) const;

  cv::Mat samples_;
  cv::Mat responses_;

  cv::Ptr<cv::ml::KNearest> model_;
  std::function<char(const cv::Mat&)> recognise_callback_;
};

#endif  // KNEAREST_H
