cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 20)

project( cpp-core-sandbox )

add_subdirectory( copy-elision )
add_subdirectory( stack-unwind )
add_subdirectory( common )
