cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

project(cpp-core-sandbox)

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

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(copy-elision)
add_subdirectory(stack-unwind)
add_subdirectory(move-semantics)
add_subdirectory(common)
add_subdirectory(random-access-containers-traversal)
add_subdirectory(multiple-inheritance)
add_subdirectory(templates-playground)
add_subdirectory(recursion-without-recursive-fn)
add_subdirectory(multithreading-sandbox)
add_subdirectory(optional-playground)
add_subdirectory(new-playground)
add_subdirectory(streambuf)
