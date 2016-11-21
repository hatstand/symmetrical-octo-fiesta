#include <jni.h>

#include <type_traits>
#include <vector>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "knearest.h"

using std::bind;
using std::extent;
using std::function;
using std::placeholders::_1;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {

string JStringToStdString(JNIEnv* env, jstring input) {
  unique_ptr<const char, function<void(const char*)>> utf(
      env->GetStringUTFChars(input, nullptr),
      bind(&JNIEnv::ReleaseStringUTFChars, env, input, _1));
  return string(utf.get(), env->GetStringUTFLength(input));
}

jlong loadModel(JNIEnv* env, jclass, jstring path_jni, jobject asset_manager) {
  const string path = JStringToStdString(env, path_jni);
  AAssetManager* assets = AAssetManager_fromJava(env, asset_manager);
  unique_ptr<AAsset, decltype(&AAsset_close)> asset(
      AAssetManager_open(assets, path.c_str(), AASSET_MODE_BUFFER),
      AAsset_close);
  off64_t length = AAsset_getLength64(asset.get());
  vector<char> buffer;
  buffer.resize(length);
  int read = AAsset_read(asset.get(), buffer.data(), length);
  if (read != length) {
    __android_log_print(ANDROID_LOG_ERROR, "cheating_native",
                        "Failed to read model");
  }

  KNearest* nearest = new KNearest;
  cv::String data(buffer.data(), buffer.size());
  if (!nearest->LoadFromString(data)) {
    __android_log_print(ANDROID_LOG_ERROR, "cheating_native",
                        "Failed to load model");
  }
  return reinterpret_cast<jlong>(nearest);
}

jbyteArray recogniseGrid(JNIEnv* env, jclass, jbyteArray data) {
  jbyte* bytes = env->GetByteArrayElements(data, nullptr);
  jsize length = env->GetArrayLength(data);
  vector<char> input(bytes, bytes + length);
  cv::Mat image = cv::imdecode(cv::InputArray(input), 0);
  env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
  if (!image.data) {
    __android_log_print(ANDROID_LOG_ERROR, "cheating_native",
                        "Failed to load image");
  }
  return nullptr;
}

static const JNINativeMethod kMethods[] = {
    {"loadModel", "(Ljava/lang/String;Landroid/content/res/AssetManager;)J",
     reinterpret_cast<void*>(&loadModel)}};

}  // namespace

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  JNIEnv* env = nullptr;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  jclass clazz = env->FindClass("com/purplehatstands/wwf/Model");
  env->RegisterNatives(clazz, kMethods, extent<decltype(kMethods)>::value);

  return JNI_VERSION_1_6;
}
