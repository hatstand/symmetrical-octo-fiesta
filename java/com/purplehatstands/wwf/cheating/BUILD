android_binary(
    name = "cheating",
    srcs = ["MainActivity.java"],
    assets = [":assets"],
    assets_dir = "assets",
    manifest = "AndroidManifest.xml",
    resource_files = glob(["res/**/*"]),
    deps = [
        ":Grid",
        ":jni",
        ":protos",
        "//:knearest",
        "//:scrabble",
        "@org_pubref_rules_protobuf//java:grpc_compiletime_deps",
        "@org_pubref_rules_protobuf//java:netty_runtime_deps",
    ],
)

android_library(
    name = "Grid",
    srcs = ["Grid.java"],
    deps = [
        ":protos",
        "@com_google_guava_guava//jar",
        "@org_pubref_rules_protobuf//java:grpc_compiletime_deps",
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
    srcs = glob(["*.cpp"]),
    linkopts = [
        "-landroid",
    ],
    deps = [
        "//:knearest",
        "//:messages_lib",
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
