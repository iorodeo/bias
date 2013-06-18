cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(blob_finder)
add_executable(blob_finder blob_finder_main.cpp blob_finder.cpp)
target_link_libraries(blob_finder ${bias_ext_link_LIBS} bias_camera_facade)



