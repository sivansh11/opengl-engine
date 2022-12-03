cmake_minimum_required(VERSION 3.10)

project(bvh_test)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(bvh_test ${SRC_FILES})

include_directories(bvh_test
    ../../core
    ../../deps/glfw/include
)

target_link_libraries(bvh_test
    core
)