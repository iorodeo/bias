cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bais)

# Backend camera library options
# -----------------------------------------------------------------------------
option(with_fc2 "include the FlyCapture2 backend" ON)
if(with_fc2)
    message(STATUS "with_fc2=on")
endif()

option(with_dc1394 "include the libdc1394 backend" OFF)
if(with_dc1394)
    message(STATUS "with_dc1394=on")
endif()


if( NOT( with_fc2 OR with_dc1394 ) )
    message(FATAL_ERROR "their must be at least one camera backend")
endif()


# Opencv library
# -----------------------------------------------------------------------------
find_package( OpenCV REQUIRED )

if(${OpenCV_FOUND})
    message(STATUS "OpenCV found")
else()
    message(STATUS "OpenCv not found")
endif()


# FlyCapture2 library location - replace this this custom find_package
# -----------------------------------------------------------------------------
if(with_fc2)
    set(flycapture2_dir "C:/Program Files/Point Grey Research/FlyCapture2")
    set(flycapture2_c_lib "${flycapture2_dir}/lib64/C/FlyCapture2_C.lib")
    set(flycapture2_include "${flycapture2_dir}/include")
endif()


# Compiler flags
# -----------------------------------------------------------------------------
set(CMAKE_CXX_FLAGS "-std=gnu++0x")


# Definitions 
# -----------------------------------------------------------------------------
add_definitions(-DDebug)

if(with_fc2)
    add_definitions(-DWITH_FC2)
endif()

if(with_dc1394)
    add_definitions(-DWITH_DC1394)
endif()


# Include directories
# -----------------------------------------------------------------------------
set(bias_include_dir "./src/backend/base" "./src/core")
if(with_fc2)
    set(
        bias_include_dir 
        ${bias_include_dir} 
        ${flycapture2_include} 
        "./src/backend/fc2"
        )
endif()

if(with_dc1394)
    set(
        bias_include_dir 
        ${bias_include_dir} 
        "./src/backend/dc1394"
        )
endif()

include_directories(${bias_include_dir})


# External link libraries
# -----------------------------------------------------------------------------
set(bias_ext_link_libs ${OpenCV_LIBS})

if(with_fc2)
    set(bias_ext_link_libs ${bias_ext_link_libs} ${flycapture2_c_lib})
endif()

if(with_dc1394)
    set(bias_ext_link_libs ${bias_ext_link_libs} dc1394)
endif()


# Output directory for executables - puts executables in build directory
# -----------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add project subdirectories
# -----------------------------------------------------------------------------
add_subdirectory("src/backend/base") 

if(with_fc2) 
    add_subdirectory("src/backend/fc2") 
endif()

if(with_dc1394) 
    add_subdirectory("src/backend/dc1394")
endif() 

add_subdirectory("src/core")
add_subdirectory("src/test")

