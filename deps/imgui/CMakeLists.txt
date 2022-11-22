cmake_minimum_required(VERSION 3.10)

project(imgui)

set(SRC_FILES imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_glfw.cpp  imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui.cpp imgui/misc/cpp/imgui_stdlib.cpp)

add_library(imgui ${SRC_FILES})

target_include_directories(imgui
    PUBLIC imgui
    PUBLIC ../glfw/include
)

