cmake_minimum_required(VERSION 3.10)

set( APP_NAME multithreading-sandbox)
project( ${APP_NAME} )

set(CMAKE_CXX_STANDARD 20)
add_executable( ${APP_NAME} ${APP_NAME}.cpp )
target_link_libraries(${APP_NAME} PRIVATE cpp-core-common )
target_include_directories( ${APP_NAME} PRIVATE ../common )
