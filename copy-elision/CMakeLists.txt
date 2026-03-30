cmake_minimum_required(VERSION 3.10)

project( copy-elision )

add_compile_options(-Wno-pessimizing-move)
set(CMAKE_CXX_STANDARD 23)
add_executable( cpp-core-copy-elision cpp-core-copy-elision.cpp )
target_link_libraries( cpp-core-copy-elision PRIVATE cpp-core-common )
target_include_directories( cpp-core-copy-elision PRIVATE ../common )
