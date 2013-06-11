cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_demo)
add_executable(bias_demo bias_demo.cpp)
target_link_libraries(bias_demo ${bias_ext_link_LIBS} bias_camera_facade)



