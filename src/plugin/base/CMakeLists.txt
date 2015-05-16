cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_plugin)
if (POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(
    bias_plugin_HEADERS 
    bias_plugin.hpp
    ../../gui/camera_window.hpp
    )

set(
    bias_plugin_SOURCES 
    bias_plugin.cpp
    )

qt5_wrap_ui(ui_headers ../../gui/camera_window.ui)

qt5_wrap_cpp(bias_plugin_HEADERS_MOC ${bias_plugin_HEADERS})

add_library(
    bias_plugin 
    ${bias_plugin_HEADERS_MOC}
    ${bias_plugin_SOURCES} 
    ${ui_headers}
    )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(bias_plugin ${QT_LIBRARIES})

qt5_use_modules(bias_plugin Core Widgets Gui)

