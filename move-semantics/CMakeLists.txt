cmake_minimum_required(VERSION 3.10)

project( move-semantics )

set(CMAKE_CXX_STANDARD 20)
add_executable( cpp-core-move-semantics cpp-core-move-semantics.cpp )
target_link_libraries( cpp-core-move-semantics PRIVATE cpp-core-common )
target_include_directories( cpp-core-move-semantics PRIVATE ../common )
