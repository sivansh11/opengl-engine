cmake_minimum_required(VERSION 3.10)

project(editor)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(editor ${SRC_FILES})

include_directories(editor
    ../../engine
    ../../deps/glfw/include
)

target_link_libraries(editor
    engine
)