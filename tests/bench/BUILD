load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_test(
    name = "bench_dist",
    size = "small",
    srcs = ["bench.cpp"],
    deps = [
        "//lib:avxcircdist",
        "//utils/catch2:catch2",
    ],
    copts = [
        "-march=native",
        "-std=c++20"
    ]
)