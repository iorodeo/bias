cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(stampede_plugin)
if (POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(
    stampede_plugin_FORMS 
    stampede_plugin.ui
    stampede_event_widget.ui
    )

set(
    stampede_plugin_HEADERS 
    stampede_plugin.hpp
    stampede_event_widget.hpp
    )

set(
    stampede_plugin_SOURCES 
    stampede_plugin.cpp
    stampede_event_widget.cpp
    )


qt5_wrap_ui(stampede_plugin_FORMS_HEADERS ${stampede_plugin_FORMS}) 
qt5_wrap_cpp(stampede_plugin_HEADERS_MOC ${stampede_plugin_HEADERS})

add_library(
    stampede_plugin 
    ${stampede_plugin_HEADERS_MOC}
    ${stampede_plugin_FORMS_HEADERS}
    ${stampede_plugin_SOURCES} 
    )

add_dependencies(stampede_plugin ${stampede_plugin_FORMS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(stampede_plugin ${QT_LIBRARIES} bias_plugin)

qt5_use_modules(stampede_plugin Core Widgets Gui)

set(stampede_plugin_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "stampede include directories")

