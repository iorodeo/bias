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
add_EXECUTABLE(test_serial test_serial.cpp)


