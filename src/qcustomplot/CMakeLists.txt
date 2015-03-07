cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(with_qt_gui)
    project(qcustomplot)
    if (POLICY CMP0020)
        cmake_policy(SET CMP0020 NEW)
    endif()
    
    set(
        qcustomplot_HEADERS
        qcustomplot.h
        )
    
    set(
        qcustomplot_SOURCES
        qcustomplot.cpp
        )
    
    qt5_wrap_cpp(qcustomplot_HEADERS_MOC ${qcustomplot_HEADERS})
    
    add_library(
        qcustomplot
        ${qcustomplot_HEADERS_MOC}
        ${qcustomplot_SOURCES} 
        )
    
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(.)
    target_link_libraries(
        qcustomplot 
        ${QT_LIBRARIES} 
        ${bias_ext_link_LIBS} 
        bias_camera_facade
        )

    qt5_use_modules(qcustomplot Core Widgets Gui PrintSupport)

endif()

