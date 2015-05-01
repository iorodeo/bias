cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(grab_detector_plugin)
if (POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(
    grab_detector_plugin_FORMS 
    grab_detector_plugin.ui
    )

set(
    grab_detector_plugin_HEADERS 
    grab_detector_config.hpp
    grab_detector_plugin.hpp
    pulse_device.hpp
    )

set(
    grab_detector_plugin_SOURCES 
    grab_detector_config.cpp
    grab_detector_plugin.cpp
    pulse_device.cpp
    )


qt5_wrap_ui(grab_detector_plugin_FORMS_HEADERS ${grab_detector_plugin_FORMS}) 
qt5_wrap_cpp(grab_detector_plugin_HEADERS_MOC ${grab_detector_plugin_HEADERS})

add_library(
    grab_detector_plugin 
    ${grab_detector_plugin_HEADERS_MOC}
    ${grab_detector_plugin_FORMS_HEADERS}
    ${grab_detector_plugin_SOURCES} 
    )

add_dependencies(grab_detector_plugin ${grab_detector_plugin_FORMS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(grab_detector_plugin ${QT_LIBRARIES} bias_plugin bias_utility bias_camera_facade qcustomplot)

qt5_use_modules(grab_detector_plugin Core Widgets Gui PrintSupport SerialPort)

set(grab_detector_plugin_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "grab detector  include directories")

