cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_backend_base)

set(bias_backend_base_SOURCE camera_device.cpp)

add_library( bias_backend_base ${bias_backend_base_SOURCE})

