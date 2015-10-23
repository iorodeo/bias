cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bias_gui)
if (POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(
    bias_gui_FORMS 
    camera_window.ui 
    property_dialog.ui
    timer_settings_dialog.ui
    logging_settings_dialog.ui
    format7_settings_dialog.ui
    alignment_settings_dialog.ui
    auto_naming_dialog.ui
    ../plugin/stampede/stampede_plugin.ui
    ../plugin/grab_detector/grab_detector_plugin.ui
    )

set(
    bias_gui_HEADERS 
    camera_window.hpp 
    validators.hpp
    image_grabber.hpp
    image_logger.hpp
    image_dispatcher.hpp
    video_writer.hpp
    video_writer_params.hpp
    video_writer_bmp.hpp
    video_writer_jpg.hpp
    video_writer_avi.hpp
    video_writer_fmf.hpp
    video_writer_ufmf.hpp
    background_data_ufmf.hpp
    background_histogram_ufmf.hpp
    background_median_ufmf.hpp
    compressed_frame_ufmf.hpp
    compressed_frame_jpg.hpp
    compressor_ufmf.hpp
    compressor_jpg.hpp
    fps_estimator.hpp
    affinity.hpp
    property_dialog.hpp
    timer_settings_dialog.hpp
    logging_settings_dialog.hpp
    format7_settings_dialog.hpp
    ext_ctl_http_server.hpp
    alignment_settings.hpp
    alignment_settings_dialog.hpp
    auto_naming_dialog.hpp
    auto_naming_options.hpp
    plugin_handler.hpp
    )

set(
    bias_gui_SOURCES 
    main.cpp 
    camera_window.cpp 
    validators.cpp
    image_grabber.cpp
    image_logger.cpp
    image_dispatcher.cpp
    video_writer.cpp
    video_writer_params.cpp
    video_writer_bmp.cpp
    video_writer_jpg.cpp
    video_writer_avi.cpp
    video_writer_fmf.cpp
    video_writer_ufmf.cpp
    background_data_ufmf.cpp
    background_histogram_ufmf.cpp
    background_median_ufmf.cpp
    compressed_frame_ufmf.cpp
    compressed_frame_jpg.cpp
    compressor_ufmf.cpp
    compressor_jpg.cpp
    fps_estimator.cpp
    affinity.cpp
    property_dialog.cpp
    timer_settings_dialog.cpp
    logging_settings_dialog.cpp
    format7_settings_dialog.cpp
    ext_ctl_http_server.cpp
    alignment_settings.cpp
    alignment_settings_dialog.cpp
    auto_naming_dialog.cpp
    auto_naming_options.cpp
    plugin_handler.cpp
    )

qt5_wrap_ui(bias_gui_FORMS_HEADERS ${bias_gui_FORMS}) 
qt5_wrap_cpp(bias_gui_HEADERS_MOC ${bias_gui_HEADERS})

add_executable(
    test_gui 
    ${bias_gui_HEADERS_MOC}
    ${bias_gui_FORMS_HEADERS} 
    ${bias_gui_SOURCES} 
    )

add_dependencies(test_gui ${bias_gui_FORMS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
target_link_libraries(
    test_gui 
    ${QT_LIBRARIES} 
    ${bias_ext_link_LIBS} 
    bias_camera_facade
    bias_utility
    stampede_plugin
    grab_detector_plugin
    )

qt5_use_modules(test_gui Core Gui Widgets Network PrintSupport SerialPort)

