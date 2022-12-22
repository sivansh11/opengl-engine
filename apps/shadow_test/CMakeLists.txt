cmake_minimum_required(VERSION 3.10)

project(shadow_test)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(shadow_test ${SRC_FILES})

include_directories(shadow_test
    ../../core
    ../../deps/glfw/include
)

target_link_libraries(shadow_test
    core
)