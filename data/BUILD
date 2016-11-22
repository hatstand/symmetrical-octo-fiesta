package(default_visibility = ["//visibility:public"])

genrule(
    name = "gen_model",
    srcs = glob(["training/**/*.png"]),
    outs = ["model"],
    cmd = "$(location //:train_model) --output $@ $(SRCS)",
    tools = ["//:train_model"],
)

exports_files(["word_list"])
