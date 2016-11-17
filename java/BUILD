android_binary(
    name = "foo",
    srcs = glob(["**/*.java"]),
    manifest = "AndroidManifest.xml",
    deps = [
        ":protos",
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
