workspace(name = "avxdist")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib", 
    sha256 = "cd55a062e763b9349921f0f5db8c3933288dc8ba4f76dd9416aac68acee3cb94", 
    urls = [ 
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz", 
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz", 
    ], 
) 
  
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace() 
  
http_archive( 
    name = "catch2", 
    sha256 = "122928b814b75717316c71af69bd2b43387643ba076a6ec16e7882bfb2dfacbb", 
    strip_prefix = "Catch2-3.4.0", 
    urls = ["https://github.com/catchorg/Catch2/archive/refs/tags/v3.4.0.tar.gz"], 
)
  
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
