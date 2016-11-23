package(default_visibility = ["//java:__subpackages__"])

cc_library(
    name = "images",
    srcs = ["images.cpp"],
    hdrs = ["images.h"],
    copts = [
        "-Iexternal/local_jdk/include",
    ],
    deps = [
        "@opencv//:opencv_core",
        "@opencv//:opencv_imgcodecs",
    ] + select({
        "//config:android": [],
        "//conditions:default": [":jni"],
    }),
)

cc_library(
    name = "jni",
    hdrs = [
        ":jni_md",
        "//external:jni_header",
    ],
    includes = ["."],
)

genrule(
    name = "jni_md",
    srcs = ["//external:jni_md_header-linux"],
    outs = ["jni_md.h"],
    cmd = "cp $< $@",
)
