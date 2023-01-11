cmake_minimum_required(VERSION 3.10)

project(vxgi)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(vxgi ${SRC_FILES})

include_directories(vxgi
    ../../engine
    ../../deps/glfw/include
)

target_link_libraries(vxgi
    engine
)