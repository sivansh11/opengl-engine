cmake_minimum_required(VERSION 3.10)

project(testing_app)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

add_executable(testing_app ${SRC_FILES})

include_directories(testing_app
    ../../core
    ../../deps/glfw/include
)

target_link_libraries(testing_app
    core
)