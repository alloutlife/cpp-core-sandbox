cmake_minimum_required(VERSION 3.10)

set( APP_NAME recursion-without-recursive-fn )
project( ${APP_NAME} )

set(CMAKE_CXX_STANDARD 20)
add_executable( ${APP_NAME} ${APP_NAME}.cpp )
target_link_libraries( recursion-without-recursive-fn PRIVATE cpp-core-common )
target_include_directories( recursion-without-recursive-fn PRIVATE ../common )
