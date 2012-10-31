CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(bais)

# Camera libraries
# -----------------------------------------------------------------------------
SET(with_fc2 false)
SET(with_dc1394 true)


# FlyCapture2 library location
# -----------------------------------------------------------------------------
IF(with_fc2)
    SET(flycapture2_dir "C:/Program Files/Point Grey Research/FlyCapture2")
    SET(flycapture2_c_lib "${flycapture2_dir}/lib64/C/FlyCapture2_C.lib")
    SET(flycapture2_include "${flycapture2_dir}/include")
ENDIF()


# Source files
# -----------------------------------------------------------------------------
SET(bias_src guid.cpp camera.cpp)
IF(with_fc2) 
    SET(
        bias_src ${bias_src} 
        utils_fc2.cpp 
        guid_device_fc2.cpp
        camera_device_fc2.cpp 
        test_fc2.cpp
        )
ENDIF()
If(with_dc1394)
    SET(
        bias_src ${bias_src} 
        guid_device_dc1394.cpp
        camera_device_dc1394.cpp 
        test_dc1394.cpp)
ENDIF()


# Compiler flags
# -----------------------------------------------------------------------------
SET(CMAKE_CXX_FLAGS "-std=gnu++0x")


# Definitions 
# -----------------------------------------------------------------------------
ADD_DEFINITIONS(-DDebug)
IF(with_fc2)
    ADD_DEFINITIONS(-DWITH_FC2 -DDebug)
ENDIF()
IF(with_dc1394)
    ADD_DEFINITIONS(-DWITH_DC1394)
ENDIF()


# Include directories
# -----------------------------------------------------------------------------
If(with_fc2)
    INCLUDE_DIRECTORIES(${flycapture2_include})
ENDIF()

ADD_EXECUTABLE(test ${bias_src})


# Link libraries
# -----------------------------------------------------------------------------
IF(with_fc2)
    TARGET_LINK_LIBRARIES(test ${flycapture2_c_lib})
ENDIF()
