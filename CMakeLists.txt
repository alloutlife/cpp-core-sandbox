cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 20)

project( cpp-core-sandbox )

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(
        -Wall -Wextra -pedantic
        # -Werror  <-- a must for commercial projects. However we need some warnings remain
    )
endif()

add_subdirectory( copy-elision )
add_subdirectory( stack-unwind )
add_subdirectory( move-semantics )
add_subdirectory( common )
