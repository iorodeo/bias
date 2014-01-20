cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_utility)

set(
    bias_utility_HEADERS
    mat_to_qimage.hpp
    blob_data.hpp
    blob_finder.hpp
    blob_finder_param.hpp
    json.hpp
    json_utils.hpp
    rtn_status.hpp
    basic_image_proc.hpp
    basic_http_server.hpp
    )

set(
    bias_utility_SOURCES
    mat_to_qimage.cpp
    blob_data.cpp
    blob_finder.cpp
    blob_finder_param.cpp
    json.cpp
    json_utils.cpp
    basic_image_proc.cpp
    basic_http_server.cpp
    )

qt4_wrap_cpp(bias_utility_HEADERS_MOC ${bias_utility_HEADERS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_library(
    bias_utility
    ${bias_utility_HEADERS_MOC}
    ${bias_utility_SOURCES} 
    )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(
    bias_utility 
    ${QT_LIBRARIES} 
    ${bias_ext_link_LIBS} 
    bias_camera_facade
    )



