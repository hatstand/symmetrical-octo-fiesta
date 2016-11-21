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
#include "scrabble.h"

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

cv::Mat DecodeImage(JNIEnv* env, jbyteArray data) {
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
  } else {
    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "Successfully loaded image %dx%d", image.size().width,
                        image.size().height);
  }
  return image;
}

KNearest* LoadModel(JNIEnv* env, jobject asset_manager, jstring path_jni) {
  // Load K-Nearest-Neighbours model.
  const string path = JStringToStdString(env, path_jni);
  AAssetManager* assets = AAssetManager_fromJava(env, asset_manager);
  vector<char> model_data = LoadAsset(assets, path);
  cv::String cv_data(model_data.data(), model_data.size());
  KNearest* nearest = new KNearest;
  if (!nearest->LoadFromString(cv_data)) {
    __android_log_print(ANDROID_LOG_ERROR, "cheating_native",
                        "Failed to load model");
    return nullptr;
  }
  __android_log_print(ANDROID_LOG_INFO, kTag, "Loaded model of size %d",
                      cv_data.size());
  return nearest;
}

jbyteArray RecogniseGrid(JNIEnv* env, jclass, jobject asset_manager,
                         jstring path_jni, jbyteArray data) {
  unique_ptr<KNearest> nearest(LoadModel(env, asset_manager, path_jni));
  cv::Mat image = DecodeImage(env, data);
  Recogniser recogniser(*nearest);
  __android_log_print(ANDROID_LOG_INFO, kTag, "Recognising grid");
  vector<char> grid = recogniser.RecogniseGrid(image);

  jbyteArray ret = env->NewByteArray(grid.size());
  jbyte* ret_data = env->GetByteArrayElements(ret, nullptr);
  copy(grid.begin(), grid.end(), ret_data);
  env->ReleaseByteArrayElements(ret, ret_data, 0);
  return ret;
}

jbyteArray RecogniseRack(JNIEnv* env, jclass, jobject asset_manager,
                         jstring path_jni, jbyteArray data) {
  unique_ptr<KNearest> nearest(LoadModel(env, asset_manager, path_jni));
  cv::Mat image = DecodeImage(env, data);
  Recogniser recogniser(*nearest);
  __android_log_print(ANDROID_LOG_INFO, kTag, "Recognising rack");
  vector<char> rack = recogniser.RecogniseRack(image);

  jbyteArray ret = env->NewByteArray(rack.size());
  jbyte* ret_data = env->GetByteArrayElements(ret, nullptr);
  copy(rack.begin(), rack.end(), ret_data);
  env->ReleaseByteArrayElements(ret, ret_data, 0);
  return ret;
}

vector<string> LoadWords(JNIEnv* env, jobjectArray words) {
  vector<string> ret;
  const int length = env->GetArrayLength(words);
  for (jsize i = 0; i < length; ++i) {
    jstring s = (jstring)env->GetObjectArrayElement(words, i);
    ret.push_back(JStringToStdString(env, s));
    env->DeleteLocalRef(s);
  }
  return ret;
}

void Solve(JNIEnv* env, jclass, jobject asset_manager, jstring path_jni,
           jbyteArray data, jobjectArray words) {
  unique_ptr<KNearest> nearest(LoadModel(env, asset_manager, path_jni));
  cv::Mat image = DecodeImage(env, data);
  Recogniser recogniser(*nearest);
  __android_log_print(ANDROID_LOG_INFO, kTag, "Recognising rack");
  vector<char> grid = recogniser.RecogniseGrid(image);
  vector<char> rack = recogniser.RecogniseRack(image);

  Scrabble scrabble(grid, LoadWords(env, words));
  vector<Scrabble::Solution> solutions = scrabble.FindBestMove(rack);
  for (const auto& solution : solutions) {
    __android_log_print(ANDROID_LOG_INFO, kTag, "Play %s at (%d,%d)",
                        solution.word().c_str(), solution.x(), solution.y());
  }
}

static const JNINativeMethod kGridMethods[] = {
    {"recogniseGrid",
     "(Landroid/content/res/AssetManager;Ljava/lang/String;[B)[B",
     reinterpret_cast<void*>(&RecogniseGrid)},
    {"recogniseRack",
     "(Landroid/content/res/AssetManager;Ljava/lang/String;[B)[B",
     reinterpret_cast<void*>(&RecogniseRack)},
    {"solve",
     "(Landroid/content/res/AssetManager;Ljava/lang/String;[B[Ljava/lang/"
     "String;)V",
     reinterpret_cast<void*>(&Solve)},
};

}  // namespace

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  JNIEnv* env = nullptr;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  jclass grid_clazz = env->FindClass("com/purplehatstands/wwf/Grid");
  env->RegisterNatives(grid_clazz, kGridMethods,
                       extent<decltype(kGridMethods)>::value);

  return JNI_VERSION_1_6;
}
