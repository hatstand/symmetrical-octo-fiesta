#ifndef IMAGES_H
#define IMAGES_H

#include <jni.h>

#include <opencv2/core/core.hpp>

cv::Mat DecodeImage(JNIEnv* env, jbyteArray data);

#endif  // IMAGES_H
