cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_backend_spin)

set(
    bias_backend_spin_SOURCE 
    utils_spin.cpp 
    guid_device_spin.cpp 
    camera_device_spin.cpp
    camera_info_spin.cpp
    node_map_utils.cpp
    node_map_spin.cpp
    base_node_spin.cpp
    string_node_spin.cpp
    enum_node_spin.cpp
    )

add_library(bias_backend_spin ${bias_backend_spin_SOURCE})

target_link_libraries(
    bias_backend_spin 
    ${bias_ext_link_LIBS} 
    bias_backend_base 
    bias_camera_facade
    )
