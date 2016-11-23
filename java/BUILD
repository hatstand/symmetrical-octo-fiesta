android_binary(
    name = "cheating",
    assets = [":assets"],
    assets_dir = "assets",
    manifest = "AndroidManifest.xml",
    deps = [
        ":jni",
        ":protos",
        "//:knearest",
        "//:scrabble",
        "//java/com/purplehatstands/wwf:MainActivity",
    ],
)

android_library(
    name = "protos",
    srcs = ["//:service_java"],
    visibility = ["//java:__subpackages__"],
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
        "//:recogniser",
        "//:scrabble",
        "//java/com/purplehatstands/wwf/jni:images",
        "@opencv//:opencv_imgcodecs",
    ],
)

genrule(
    name = "assets",
    srcs = [
        "//data:model",
        "//data:word_list",
    ],
    outs = [
        "assets/model",
        "assets/word_list",
    ],
    cmd = """
        for i in $(SRCS); do
          cp $$i $(@D)/assets/
        done
    """,
)
