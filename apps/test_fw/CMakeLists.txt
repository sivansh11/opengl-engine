cmake_minimum_required(VERSION 3.10)

project(test_fw)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(test_fw ${SRC_FILES})

include_directories(test_fw
    ../../engine
    ../../deps/glfw/include
)

target_link_libraries(test_fw
    engine
)