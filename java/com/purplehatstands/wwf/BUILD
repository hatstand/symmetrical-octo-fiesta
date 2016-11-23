java_binary(
    name = "BuildModel",
    srcs = ["BuildModel.java"],
    deps = [
        "//java/com/purplehatstands/wwf/jni",
        "@com_google_guava_guava//jar",
    ],
)

android_library(
    name = "Grid",
    srcs = ["Grid.java"],
    deps = [
        "@com_google_guava_guava//jar",
    ],
)

android_library(
    name = "MainActivity",
    srcs = ["MainActivity.java"],
    visibility = ["//java:__pkg__"],
    deps = [
        ":Grid",
        "//java:protos",
        "@org_pubref_rules_protobuf//java:grpc_compiletime_deps",
        "@org_pubref_rules_protobuf//java:netty_runtime_deps",
    ],
)
