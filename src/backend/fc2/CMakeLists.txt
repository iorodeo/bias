cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_backend_fc2)

set(
    bias_backend_fc2_src 
    utils_fc2.cpp 
    guid_device_fc2.cpp 
    camera_device_fc2.cpp
    )

add_library(bias_backend_fc2 ${bias_backend_fc2_src})

target_link_libraries(
    bias_backend_fc2 
    ${bias_ext_link_libs} 
    bias_backend_base 
    bias_core
    )
