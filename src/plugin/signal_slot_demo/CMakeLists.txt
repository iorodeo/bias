cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(signal_slot_demo_plugin)
if (POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(
    signal_slot_demo_plugin_FORMS 
    signal_slot_demo_plugin.ui
    ../../gui/camera_window.ui
    )

set(
    signal_slot_demo_plugin_HEADERS 
    frame_data.hpp
    signal_slot_demo_plugin.hpp
    )

set(
    signal_slot_demo_plugin_SOURCES 
    signal_slot_demo_plugin.cpp
    )

qt5_wrap_ui(signal_slot_demo_plugin_FORMS_HEADERS ${signal_slot_demo_plugin_FORMS}) 
qt5_wrap_cpp(signal_slot_demo_plugin_HEADERS_MOC ${signal_slot_demo_plugin_HEADERS})

add_library(
    signal_slot_demo_plugin 
    ${signal_slot_demo_plugin_HEADERS_MOC}
    ${signal_slot_demo_plugin_FORMS_HEADERS}
    ${signal_slot_demo_plugin_SOURCES} 
    )

add_dependencies(signal_slot_demo_plugin ${signal_slot_demo_plugin_FORMS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(signal_slot_demo_plugin ${QT_LIBRARIES} bias_plugin bias_utility bias_camera_facade )

qt5_use_modules(signal_slot_demo_plugin Core Widgets Gui PrintSupport )

