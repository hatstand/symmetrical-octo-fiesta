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
        "@opencv//:opencv_highgui",
    ],
)

genrule(
    name = "gen_model",
    srcs = glob(["training/**/*.png"]),
    outs = ["model"],
    cmd = """
      $(location :train_model) $@ $(SRCS)
    """,
    tools = [":train_model"],
)
