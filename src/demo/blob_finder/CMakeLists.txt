cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(blob_finder)

set(
    blob_finder_HEADERS
    blob_data.hpp
    blob_finder_param.hpp
    blob_finder.hpp
    )

set(
    blob_finder_SOURCES
    blob_data.cpp
    blob_finder_main.cpp
    blob_finder_param.cpp
    blob_finder.cpp
    )

qt4_wrap_cpp(blob_finder_HEADERS_MOC ${blob_finder_HEADERS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(
    blob_finder
    ${blob_finder_HEADERS_MOC}
    ${blob_finder_SOURCES} 
    )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(
    blob_finder 
    ${QT_LIBRARIES} 
    ${bias_ext_link_LIBS} 
    bias_camera_facade
    )



