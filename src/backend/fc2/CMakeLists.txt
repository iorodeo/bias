cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_backend_fc2)

set(
    bias_backend_fc2_SOURCE 
    utils_fc2.cpp 
    guid_device_fc2.cpp 
    camera_device_fc2.cpp
    )

add_library(bias_backend_fc2 ${bias_backend_fc2_SOURCE})

target_link_libraries(
    bias_backend_fc2 
    ${bias_ext_link_LIBS} 
    bias_backend_base 
    bias_camera_facade
    )
