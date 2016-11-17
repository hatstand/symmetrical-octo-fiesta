#include "knearest.h"

#include <iostream>

#include "opencv2/imgproc/imgproc.hpp"

using std::cerr;
using std::endl;

namespace {
static const int kOcrMaxDistance = 60000;
}

KNearest::KNearest() : model_(cv::ml::KNearest::create()) {}

void KNearest::Learn(const cv::Mat& image, char c) {
  responses_.push_back(cv::Mat(1, 1, CV_32F, (float)c));
  samples_.push_back(PrepareSample(image));
}

void KNearest::Train() {
  model_->train(cv::InputArray(samples_), cv::ml::ROW_SAMPLE,
                cv::InputArray(responses_));
}

char KNearest::Recognise(const cv::Mat& image) const {
  cv::Mat results;
  cv::Mat neighbour_responses;
  cv::Mat distances;
  float result = model_->findNearest(cv::InputArray(PrepareSample(image)), 1,
                                     results, neighbour_responses, distances);
  return static_cast<char>(result);

  /*
  if (static_cast<int>(neighbour_responses.at<float>(0, 0) -
                       neighbour_responses.at<float>(0, 1)) == 0 &&
      distances.at<float>(0, 0) < kOcrMaxDistance) {
    return static_cast<char>(result);
  }
  return '?';
  */
}

cv::Mat KNearest::PrepareSample(const cv::Mat& image) const {
  cv::Mat roi;
  cv::resize(image, roi, cv::Size(20, 20));
  cv::Mat sample;
  roi.reshape(1, 1).convertTo(sample, CV_32F);
  return sample;
}

void KNearest::Save(const std::string& path) { model_->save(path); }

void KNearest::Load(const std::string& path) {
  model_ = cv::ml::KNearest::load<cv::ml::KNearest>(path);
  if (!model_) {
    cerr << "Failed to load KNearest model" << endl;
  }
}
