cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_backend_dc1394)

set(
    bias_backend_dc1394_SOURCE 
    utils_dc1394.cpp
    guid_device_dc1394.cpp 
    camera_device_dc1394.cpp
    )

add_library(bias_backend_dc1394 ${bias_backend_dc1394_SOURCE})

target_link_libraries(
    bias_backend_dc1394 
    ${bias_ext_link_LIBS} 
    bias_backend_base 
    bias_camera_facade
    )
