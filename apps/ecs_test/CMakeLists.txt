cmake_minimum_required(VERSION 3.10)

project(ecs_test)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(ecs_test ${SRC_FILES})

include_directories(ecs_test
    ../../core
    ../../deps/glfw/include
)

target_link_libraries(ecs_test
    core
)