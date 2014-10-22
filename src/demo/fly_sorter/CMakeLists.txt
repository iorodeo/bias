cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(fly_sorter)
if (POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(
    fly_sorter_FORMS 
    fly_sorter_window.ui 
    )

set(
    fly_sorter_HEADERS
    fly_sorter_window.hpp
    image_grabber.hpp
    parameters.hpp
    fast_binary_predictor.hpp
    fly_segmenter.hpp
    hog_position_fitter.hpp
    identity_tracker.hpp
    bgr_to_luv_converter.hpp
    gender_sorter.hpp
    hungarian.hpp
    ext_ctl_http_server.hpp
    )

set(
    fly_sorter_SOURCES
    fly_sorter_main.cpp
    fly_sorter_window.cpp
    image_grabber.cpp
    parameters.cpp
    fast_binary_predictor.cpp
    fly_segmenter.cpp
    hog_position_fitter.cpp
    identity_tracker.cpp
    bgr_to_luv_converter.cpp
    gender_sorter.cpp
    hungarian.cpp
    ext_ctl_http_server.cpp
    )

#qt4_wrap_ui(fly_sorter_FORMS_HEADERS ${fly_sorter_FORMS}) 
#qt4_wrap_cpp(fly_sorter_HEADERS_MOC ${fly_sorter_HEADERS})
#include(${QT_USE_FILE})
#add_definitions(${QT_DEFINITIONS})

qt5_wrap_ui(bias_gui_FORMS_HEADERS ${bias_gui_FORMS}) 
qt5_wrap_cpp(bias_gui_HEADERS_MOC ${bias_gui_HEADERS})
include_directories(${Qt5Core_INCLUDES}) 
include_directories(${Qt5Widgets_INCLUDES}) 
include_directories(${Qt5Gui_INCLUDES}) 
include_directories(${Qt5Network_INCLUDES}) 
add_definitions(${Qt5Core_DEFINITIONS}) 
add_definitions(${Qt5Widgets_DEFINITIONS}) 
add_definitions(${Qt5Gui_DEFINITIONS}) 
add_definitions(${Qt5Network_DEFINITIONS}) 

add_executable(
    fly_sorter
    ${fly_sorter_FORMS_HEADERS}
    ${fly_sorter_HEADERS_MOC}
    ${fly_sorter_SOURCES} 
    )

add_dependencies(fly_sorter ${fly_sorter_FORMS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(
    fly_sorter 
    ${QT_LIBRARIES} 
    ${bias_ext_link_LIBS} 
    bias_camera_facade
    bias_utility
    )



