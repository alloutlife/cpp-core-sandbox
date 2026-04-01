cmake_minimum_required(VERSION 3.27)

project(cpp-core-sandbox)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(benchmark)

add_subdirectory(copy-elision)
add_subdirectory(create-once-return)
add_subdirectory(stack-unwind)
add_subdirectory(move-semantics)
add_subdirectory(common)
add_subdirectory(random-access-containers-traversal)
add_subdirectory(multiple-inheritance)
add_subdirectory(templates-playground)
add_subdirectory(recursion-without-recursive-fn)
add_subdirectory(multithreading-sandbox)
add_subdirectory(optional-playground)
add_subdirectory(ranges-playground)
add_subdirectory(new-playground)
add_subdirectory(streambuf)
