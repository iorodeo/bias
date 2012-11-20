cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_core)

set(bias_core_src 
    utils.cpp
    guid.cpp 
    camera.cpp 
    camera_finder.cpp 
    exception.cpp
    property.cpp)

add_library(bias_core ${bias_core_src})

set(bias_core_link_libs ${bias_ext_link_libs} bias_backend_base)

if(with_fc2)
    set(bias_core_link_libs ${bias_core_link_libs} bias_backend_fc2)
endif()

if(with_dc1394)
    set(bias_core_link_libs ${bias_core_link_libs} bias_backend_dc1394)
endif()

target_link_libraries(bias_core ${bias_core_link_libs})

