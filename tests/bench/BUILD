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
        "-std=c++20",
        "-O3", # Ozon
    ]
)

cc_test(
    name = "bench_dist_no3",
    size = "small",
    srcs = ["bench.cpp"],
    deps = [
        "//lib:avxcircdist",
        "//utils/catch2:catch2",
    ],
    copts = [
        "-march=native",
        "-std=c++20",
    ]
)

cc_test(
    name = "test_dist",
    size = "small",
    srcs = ["tests.cpp"],
    deps = [
        "//lib:avxcircdist",
        "//utils/catch2:catch2",
    ],
    copts = [
        "-march=native",
        "-std=c++20",
        "-O3",
    ]
)