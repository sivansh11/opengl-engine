cmake_minimum_required(VERSION 3.10)

project(core)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_library(core ${SRC_FILES})

target_link_libraries(core
    glfw
    glad
    glm
    stb_image
    imgui
    assimp
    pybind11::embed
)