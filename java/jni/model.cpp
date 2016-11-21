#include <jni.h>

#include <algorithm>
#include <type_traits>
#include <vector>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "knearest.h"
#include "recogniser.h"

using std::bind;
using std::copy;
using std::extent;
using std::function;
using std::placeholders::_1;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {

static const char* kTag = "cheating_native";

string JStringToStdString(JNIEnv* env, jstring input) {
  unique_ptr<const char, function<void(const char*)>> utf(
      env->GetStringUTFChars(input, nullptr),
      bind(&JNIEnv::ReleaseStringUTFChars, env, input, _1));
  return string(utf.get(), env->GetStringUTFLength(input));
}

vector<char> LoadAsset(AAssetManager* asset_manager, const std::string& path) {
  unique_ptr<AAsset, decltype(&AAsset_close)> asset(
      AAssetManager_open(asset_manager, path.c_str(), AASSET_MODE_BUFFER),
      AAsset_close);
  off64_t length = AAsset_getLength64(asset.get());
  vector<char> buffer;
  buffer.resize(length);
  int read = AAsset_read(asset.get(), buffer.data(), length);
  if (read != length) {
    __android_log_print(ANDROID_LOG_ERROR, "cheating_native",
                        "Failed to read asset");
  }
  return buffer;
}

jlong LoadModel(JNIEnv* env, jclass, jstring path_jni, jobject asset_manager) {
  const string path = JStringToStdString(env, path_jni);
  AAssetManager* assets = AAssetManager_fromJava(env, asset_manager);

  vector<char> buffer = LoadAsset(assets, path);

  KNearest* nearest = new KNearest;
  cv::String data(buffer.data(), buffer.size());
  if (!nearest->LoadFromString(data)) {
    __android_log_print(ANDROID_LOG_ERROR, "cheating_native",
                        "Failed to load model");
  }
  __android_log_print(ANDROID_LOG_INFO, kTag, "Loaded model of size: %d",
                      data.size());
  return reinterpret_cast<jlong>(nearest);
}

jbyteArray RecogniseGrid(JNIEnv* env, jclass, jobject asset_manager,
                         jstring path_jni, jbyteArray data) {
  // Decode image into opencv readable form.
  jbyte* bytes = env->GetByteArrayElements(data, nullptr);
  jsize length = env->GetArrayLength(data);
  vector<char> input(bytes, bytes + length);
  __android_log_print(ANDROID_LOG_INFO, kTag,
                      "Trying to decode image of size %d/%d", length,
                      input.size());
  cv::Mat image = cv::imdecode(input, 0);
  env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
  if (!image.data) {
    __android_log_print(ANDROID_LOG_ERROR, kTag, "Failed to load image");
    return nullptr;
  }
  __android_log_print(ANDROID_LOG_INFO, kTag, "Successfully loaded image %dx%d",
                      image.size().width, image.size().height);

  // Load K-Nearest-Neighbours model.
  const string path = JStringToStdString(env, path_jni);
  AAssetManager* assets = AAssetManager_fromJava(env, asset_manager);
  vector<char> model_data = LoadAsset(assets, path);
  cv::String cv_data(model_data.data(), model_data.size());
  KNearest nearest;
  if (!nearest.LoadFromString(cv_data)) {
    __android_log_print(ANDROID_LOG_ERROR, "cheating_native",
                        "Failed to load model");
    return nullptr;
  }
  __android_log_print(ANDROID_LOG_INFO, kTag, "Loaded model of size %d",
                      cv_data.size());
  Recogniser recogniser(nearest);
  __android_log_print(ANDROID_LOG_INFO, kTag, "Recognising grid");
  vector<char> grid = recogniser.RecogniseGrid(image);

  jbyteArray ret = env->NewByteArray(grid.size());
  jbyte* ret_data = env->GetByteArrayElements(ret, nullptr);
  copy(grid.begin(), grid.end(), ret_data);
  env->ReleaseByteArrayElements(ret, ret_data, 0);
  return ret;
}

static const JNINativeMethod kModelMethods[] = {
    {"loadModel", "(Ljava/lang/String;Landroid/content/res/AssetManager;)J",
     reinterpret_cast<void*>(&LoadModel)}};

static const JNINativeMethod kGridMethods[] = {
    {"recogniseGrid",
     "(Landroid/content/res/AssetManager;Ljava/lang/String;[B)[B",
     reinterpret_cast<void*>(&RecogniseGrid)}};

}  // namespace

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  JNIEnv* env = nullptr;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  jclass clazz = env->FindClass("com/purplehatstands/wwf/Model");
  env->RegisterNatives(clazz, kModelMethods,
                       extent<decltype(kModelMethods)>::value);

  jclass grid_clazz = env->FindClass("com/purplehatstands/wwf/Grid");
  env->RegisterNatives(grid_clazz, kGridMethods,
                       extent<decltype(kGridMethods)>::value);

  return JNI_VERSION_1_6;
}
