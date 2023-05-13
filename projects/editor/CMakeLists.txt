cmake_minimum_required(VERSION 3.10)

project(editor)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

SET(PROJECT_NAME editor)
SET_PROPERTY(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

add_executable(editor ${SRC_FILES})

include_directories(editor
    ../../engine
    ../../deps/glfw/include
    ../../deps/screen_capture_lite/include
)

target_link_libraries(editor
    engine
)