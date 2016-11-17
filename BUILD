load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cc_proto_library")
load("@org_pubref_rules_protobuf//java:rules.bzl", "java_proto_compile")

cc_binary(
    name = "words",
    srcs = ["words.cpp"],
    data = ["model"],
    deps = [
        ":knearest",
        ":scrabble",
        "@opencv//:opencv_core",
        "@opencv//:opencv_highgui",
        "@opencv//:opencv_imgproc",
        "@opencv//:opencv_ml",
    ],
)

cc_library(
    name = "scrabble",
    srcs = [
        "scrabble.cpp",
        "scrabble.h",
    ],
    deps = [
        "@dawgdic//:dawgdic",
    ],
)

cc_library(
    name = "knearest",
    srcs = [
        "knearest.cpp",
        "knearest.h",
    ],
    deps = [
        "@opencv//:opencv_core",
        "@opencv//:opencv_imgproc",
        "@opencv//:opencv_ml",
    ],
)

cc_binary(
    name = "train_model",
    srcs = ["train_model.cpp"],
    deps = [
        ":knearest",
        "//external:gflags",
        "@opencv//:opencv_highgui",
    ],
)

cc_binary(
    name = "server",
    srcs = ["server.cpp"],
    data = ["model"],
    deps = [
        ":knearest",
        ":scrabble",
        ":service",
        "//external:gflags",
        "@opencv//:opencv_imgcodecs",
    ],
)

cc_binary(
    name = "client",
    srcs = ["client.cpp"],
    deps = [
        ":service",
        "//external:gflags",
    ],
)

cc_proto_library(
    name = "service",
    protos = ["service.proto"],
    with_grpc = True,
)

java_proto_compile(
    name = "service_java",
    protos = ["service.proto"],
    visibility = ["//visibility:public"],
    with_grpc = True,
)

genrule(
    name = "gen_model",
    srcs = glob(["training/**/*.png"]),
    outs = ["model"],
    cmd = "$(location :train_model) --output $@ $(SRCS)",
    tools = [":train_model"],
)
