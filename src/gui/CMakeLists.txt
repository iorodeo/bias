cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_gui)

set(bias_gui_FORMS main_window.ui)

set(
    bias_gui_HEADERS 
    main_window.hpp 
    mat_to_qimage.hpp
    image_pool.hpp
    )

set(
    bias_gui_SOURCES 
    main.cpp 
    main_window.cpp 
    mat_to_qimage.cpp
    image_pool.cpp
    )


qt4_wrap_ui(bias_gui_FORMS_HEADERS ${bias_gui_FORMS}) 
qt4_wrap_cpp(bias_gui_HEADERS_MOC ${bias_gui_HEADERS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(
    test_gui 
    ${bias_gui_HEADERS_MOC}
    ${bias_gui_FORMS_HEADERS} 
    ${bias_gui_SOURCES} 
    )

add_dependencies(test_gui ${bias_gui_FORMS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(test_gui ${QT_LIBRARIES} ${bias_ext_link_libs} bias_core)

