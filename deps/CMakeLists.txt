cmake_minimum_required(VERSION 3.10)

project(deps)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
add_subdirectory(glad)
add_subdirectory(glm)
add_subdirectory(stb_image)
add_subdirectory(imgui)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)
add_subdirectory(pybind11)
add_subdirectory(yaml-cpp)
add_subdirectory(efsw)
add_subdirectory(ImGuizmo)
add_subdirectory(entt)
add_subdirectory(ctti)

# add_subdirectory(screen_capture_lite)
