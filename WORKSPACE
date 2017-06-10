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

new_http_archive(
    name = "libpng_http",
    build_file = "BUILD.libpng",
    sha256 = "e30bf36cd5882e017c23a5c6a79a9aa1a744dd5841bb45ff7035ec6e3b3096b8",
    strip_prefix = "libpng-1.6.29",
    url = "http://download.sourceforge.net/libpng/libpng-1.6.29.tar.gz",
)

bind(
    name = "libpng",
    actual = "@libpng_http//:libpng",
)

new_git_repository(
    name = "opencv",
    build_file = "BUILD.opencv",
    remote = "https://github.com/opencv/opencv",
    tag = "3.1.0",
)

git_repository(
    name = "org_pubref_rules_protobuf",
    commit = "404977c0290704aacca00fac8ca0ecaeda09a7cf",
    remote = "https://github.com/pubref/rules_protobuf",
)

load("@org_pubref_rules_protobuf//cpp:rules.bzl", "cpp_proto_repositories")
load("@org_pubref_rules_protobuf//java:rules.bzl", "java_proto_repositories")

cpp_proto_repositories()

java_proto_repositories()

android_sdk_repository(
    name = "androidsdk",
    api_level = 26,
    build_tools_version = "26.0.0",
)

android_ndk_repository(
    name = "androidndk",
    api_level = 25,
)

maven_jar(
    name = "jsr250",
    artifact = "javax.annotation:jsr250-api:1.0",
)
