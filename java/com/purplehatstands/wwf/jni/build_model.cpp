#include <jni.h>

namespace {
void Recognise(JNIEnv* env, jclass clazz, jbyteArray pngData) {}

static const JNINativeMethod kMethods[] = {
    {"recognise", "([B)V", reinterpret_cast<void*>(&Recognise)}};
}  // namespace

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  JNIEnv* env = nullptr;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  jclass clazz = env->FindClass("com/purplehatstands/wwf/BuildModel");
  env->RegisterNatives(clazz, kMethods, extend<decltype(kMethods)>::value);
  return JNI_VERSION_1_6;
}
