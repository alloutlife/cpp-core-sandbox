cmake_minimum_required(VERSION 3.10)

enable_testing()

set(APP_NAME new-playground)
project(${APP_NAME})

set(CMAKE_CXX_STANDARD 20)
add_executable(${APP_NAME} ${APP_NAME}.cpp)
add_executable(${APP_NAME}.g ${APP_NAME}.g.cpp)

target_link_libraries(
  ${APP_NAME}.g
  GTest::gtest_main
  GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(${APP_NAME}.g)