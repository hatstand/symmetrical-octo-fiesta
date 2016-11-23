#include "images.h"

#include <vector>

#include <opencv2/imgcodecs/imgcodecs.hpp>

using std::vector;

cv::Mat DecodeImage(JNIEnv* env, jbyteArray data) {
  // Decode image into opencv readable form.
  jbyte* bytes = env->GetByteArrayElements(data, nullptr);
  jsize length = env->GetArrayLength(data);
  vector<char> input(bytes, bytes + length);
  cv::Mat image = cv::imdecode(input, 0);
  env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
  return image;
}
