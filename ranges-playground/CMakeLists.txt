cmake_minimum_required(VERSION 3.17)

project(ranges-playground)

set(CMAKE_CXX_STANDARD 23)

add_library(
  ranges-playground-lib
  SHARED
  ranges_playground.cpp
)

target_include_directories(ranges-playground-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ranges-playground main.cpp)
target_link_libraries(ranges-playground PRIVATE ranges-playground-lib)

add_executable(ranges-playground-bench benchmark.cpp)
target_link_libraries(
  ranges-playground-bench
  PRIVATE
  ranges-playground-lib
  benchmark::benchmark
)
