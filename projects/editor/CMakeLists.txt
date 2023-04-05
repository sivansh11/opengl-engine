cmake_minimum_required(VERSION 3.10)

project(Editor)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(Editor ${SRC_FILES})

include_directories(Editor
    ../../engine
    ../../deps/glfw/include
)

target_link_libraries(Editor
    engine
)