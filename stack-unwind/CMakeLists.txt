cmake_minimum_required(VERSION 3.10)

project( stack-unwind )

set(CMAKE_CXX_STANDARD 20)
add_executable( cpp-core-stack-unwind cpp-core-stack-unwind.cpp )
target_link_libraries( cpp-core-stack-unwind PRIVATE cpp-core-common )
target_include_directories( cpp-core-stack-unwind PRIVATE ../common )
