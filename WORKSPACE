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
