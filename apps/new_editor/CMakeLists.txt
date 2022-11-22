cmake_minimum_required(VERSION 3.10)

project(new_editor)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(new_editor ${SRC_FILES})

include_directories(new_editor
    ../../core
    ../../deps/glfw/include
)

target_link_libraries(new_editor
    core
)