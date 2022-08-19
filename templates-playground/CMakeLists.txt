cmake_minimum_required(VERSION 3.10)

project( templates-playground )

set(CMAKE_CXX_STANDARD 14)
add_executable( templates-playground main.cpp sfinae.cpp )
