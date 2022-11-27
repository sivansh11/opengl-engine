cmake_minimum_required(VERSION 3.10)

project(python_test)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(python_test ${SRC_FILES})

include_directories(python_test
    ../../core
    ../../deps/glfw/include
)

target_link_libraries(python_test
    core
)