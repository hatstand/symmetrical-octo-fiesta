new_git_repository(
    name = "dawgdic",
    build_file = "BUILD.dawgdic",
    commit = "16ac537ba9883ff01b63b6d1fdc3072150c68fee",
    remote = "https://github.com/stil/dawgdic",
)

git_repository(
    name = "gflags_git",
    commit = "cce68f0c9c5d054017425e6e6fd54f696d36e8ee",
    remote = "https://github.com/gflags/gflags.git",
)

bind(
    name = "gflags",
    actual = "@gflags_git//:gflags",
)

new_git_repository(
    name = "opencv",
    build_file = "BUILD.opencv",
    remote = "https://github.com/opencv/opencv",
    tag = "3.1.0",
)

git_repository(
    name = "org_pubref_rules_protobuf",
    #tag = "v0.7.0",
    commit = "404977c0290704aacca00fac8ca0ecaeda09a7cf",
    remote = "https://github.com/pubref/rules_protobuf",
)

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cpp_proto_repositories")
load("@org_pubref_rules_protobuf//java:rules.bzl", "java_proto_repositories")

cpp_proto_repositories()

java_proto_repositories()

android_sdk_repository(
    name = "androidsdk",
    api_level = 25,
    build_tools_version = "25.0.0",
    path = "/usr/local/google/home/johnmaguire/build/android-sdk-linux",
)

maven_jar(
    name = "jsr250",
    artifact = "javax.annotation:jsr250-api:1.0",
)
