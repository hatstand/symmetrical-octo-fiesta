package(default_visibility = ["//java:__subpackages__"])

cc_library(
    name = "images",
    srcs = ["images.cpp"],
    hdrs = ["images.h"],
    deps = [
        ":jni",
        "@opencv//:opencv_core",
        "@opencv//:opencv_imgcodecs",
    ],
)

cc_library(
    name = "jni",
    hdrs = [
        "@local_jdk//:jni_header",
        "@local_jdk//:jni_md_header-linux",
    ],
)
