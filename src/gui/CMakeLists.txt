cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_gui)

set(bias_gui_FORMS camera_window.ui)

set(
    bias_gui_HEADERS 
    camera_window.hpp 
    lockable.hpp
    mat_to_qimage.hpp
    image_grabber.hpp
    image_logger.hpp
    image_dispatcher.hpp
    video_writer.hpp
    video_writer_bmp.hpp
    video_writer_avi.hpp
    video_writer_fmf.hpp
    video_writer_ufmf.hpp
    video_writer_ifmf.hpp
    background_data_ufmf.hpp
    background_histogram_ufmf.hpp
    background_median_ufmf.hpp
    fps_estimator.hpp
    )

set(
    bias_gui_SOURCES 
    main.cpp 
    camera_window.cpp 
    mat_to_qimage.cpp
    image_grabber.cpp
    image_logger.cpp
    image_dispatcher.cpp
    video_writer.cpp
    video_writer_bmp.cpp
    video_writer_avi.cpp
    video_writer_fmf.cpp
    video_writer_ufmf.cpp
    video_writer_ifmf.cpp
    background_data_ufmf.cpp
    background_histogram_ufmf.cpp
    background_median_ufmf.cpp
    fps_estimator.cpp
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
target_link_libraries(
    test_gui 
    ${QT_LIBRARIES} 
    ${bias_ext_link_LIBS} 
    bias_camera_facade
    )

