cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(with_fc2)
    project(bias_test_fc2)
    add_executable(test_fc2 test_fc2.cpp)
    target_link_libraries(test_fc2 ${bias_ext_link_LIBS} bias_camera_facade)

endif()

if(with_dc1394) 
    project(bias_test_dc1394)
    add_EXECUTABLE(test_dc1394 test_dc1394.cpp)
    target_link_libraries(test_dc1394 ${bias_ext_link_LIBS} bias_camera_facade)
endif()

project(bias_test_serial)
if (POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()
set(
    bias_test_serial_HEADERS 
    nano_ssr_pulse.hpp
    )

set(
    bias_test_serial_SOURCES 
    test_serial.cpp
    nano_ssr_pulse.cpp
    )
qt5_wrap_cpp(bias_test_serial_HEADERS_MOC ${bias_test_serial_HEADERS})

add_EXECUTABLE(
    test_serial 
    ${bias_test_serial_HEADERS_MOC}
    ${bias_test_serial_SOURCES} 
    )
qt5_use_modules(test_serial Core Gui Widgets SerialPort)


