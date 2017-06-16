cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_backend_base)

set(bias_backend_base_SOURCE camera_device.cpp)

add_library( bias_backend_base ${bias_backend_base_SOURCE})

# Seems to be required now for some reason? ...
#target_link_libraries(bias_backend_base  ${bias_ext_link_LIBS} bias_camera_facade)
target_link_libraries(bias_backend_base  bias_camera_facade)

