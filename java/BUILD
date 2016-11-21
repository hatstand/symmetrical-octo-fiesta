android_binary(
    name = "foo",
    srcs = glob(["**/*.java"]),
    assets = [":assets"],
    assets_dir = "assets",
    manifest = "AndroidManifest.xml",
    deps = [
        ":jni",
        ":protos",
        "//:knearest",
        "//:scrabble",
        "@org_pubref_rules_protobuf//java:grpc_compiletime_deps",
        "@org_pubref_rules_protobuf//java:netty_runtime_deps",
    ],
)

android_library(
    name = "protos",
    srcs = ["//:service_java"],
    deps = [
        "@jsr250//jar",
        "@org_pubref_rules_protobuf//java:grpc_compiletime_deps",
    ],
)

cc_library(
    name = "jni",
    srcs = glob(["jni/**/*.cpp"]),
    copts = ["--std=c++11"],
    linkopts = [
        "-landroid",
    ],
    deps = [
        "//:knearest",
        "@opencv//:opencv_imgcodecs",
    ],
)

genrule(
    name = "assets",
    srcs = ["//:model"],
    outs = ["assets/model"],
    cmd = "cp $< $@",
)
