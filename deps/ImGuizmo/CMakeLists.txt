cmake_minimum_required(VERSION 3.10)

project(ImGuizmo)

file(GLOB SRC_FILES ImGuizmo/*.cpp)

add_library(ImGuizmo
    ${SRC_FILES}
)

include_directories(ImGuizmo
    ../imgui/imgui
)

target_include_directories(ImGuizmo
    PUBLIC /ImGuizmo
)