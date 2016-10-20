#include "knearest.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>

namespace {
static const int kOcrMaxDistance = 60000;
}

void KNearest::Learn(const cv::Mat& image, char c) {
  std::cout << "Training: " << c << std::endl;
  responses_.push_back(cv::Mat(1, 1, CV_32F, (float)c));
  samples_.push_back(PrepareSample(image));
}

void KNearest::Train() { model_.reset(new CvKNearest(samples_, responses_)); }

char KNearest::Recognise(const cv::Mat& image) {
  cv::Mat results;
  cv::Mat neighbour_responses;
  cv::Mat distances;
  float result = model_->find_nearest(PrepareSample(image), 2, results,
                                      neighbour_responses, distances);

  if (static_cast<int>(neighbour_responses.at<float>(0, 0) -
                       neighbour_responses.at<float>(0, 1)) == 0 &&
      distances.at<float>(0, 0) < kOcrMaxDistance) {
    return static_cast<char>(result);
  }
  return '?';
}

cv::Mat KNearest::PrepareSample(const cv::Mat& image) {
  cv::Mat roi;
  cv::resize(image, roi, cv::Size(20, 20));
  cv::Mat sample;
  roi.reshape(1, 1).convertTo(sample, CV_32F);
  return sample;
}
