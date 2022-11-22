cmake_minimum_required(VERSION 3.10)

project(texture_test)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(texture_test ${SRC_FILES})

include_directories(texture_test
    ../../core
    ../../deps/glfw/include
)

target_link_libraries(texture_test
    core
)