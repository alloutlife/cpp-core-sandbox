cmake_minimum_required(VERSION 3.10)

set(APP_NAME streambuf-playground)
project(${APP_NAME})

set(CMAKE_CXX_STANDARD 20)
add_executable(${APP_NAME} main-streambuf.cpp)
