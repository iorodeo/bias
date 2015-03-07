cmake_minimum_required(VERSION 2.8 FATAL_ERROR) 
project(bais)

if (POLICY CMP0020) 
    cmake_policy(SET CMP0020 NEW)
endif()


# Options
# -----------------------------------------------------------------------------
option(with_qt_gui  "include image acquisition GUI"   ON) 
option(with_fc2     "include the FlyCapture2 backend" OFF)
option(with_dc1394  "include the libdc1394 backend"   ON )
option(with_demos   "include demos" OFF)
option(with_tests   "include tests" ON)

message(STATUS "Option: with_fc2     = ${with_fc2}")
message(STATUS "Option: with_dc1394  = ${with_dc1394}")
message(STATUS "Option: with_qt_gui  = ${with_qt_gui}")
message(STATUS "Option: with_demos   = ${with_demos}")
message(STATUS "Option: with_tests   = ${with_tests}") 

if( NOT( with_fc2 OR with_dc1394 ) )
    message(FATAL_ERROR "their must be at least one camera backend")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

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
    find_package( FlyCapture2 MODULE REQUIRED )
endif()

# Qt library
# ------------------------------------------------------------------------------
if(with_qt_gui)
    set(QT_BINARY_DIR "$ENV{QTDIR}/bin")

    # Old qt 4.8 build
    # ------------------------------------------------------------
    #find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
    # ------------------------------------------------------------

    find_package(Qt5Core REQUIRED) 
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Gui REQUIRED) 
    find_package(Qt5Network REQUIRED)

endif()

# Compiler flags
# -----------------------------------------------------------------------------
#set(CMAKE_CXX_FLAGS "-std=gnu++0x -O2 -Wall")
set(CMAKE_CXX_FLAGS "-std=gnu++0x -O2")
#set(CMAKE_CXX_FLAGS "-std=gnu++0x -mwindows -O2")


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

include_directories("./src/backend/base")
include_directories("./src/facade")
include_directories("./src/utility")
include_directories(${bias_plugin_INCLUDE_DIRS})
include_directories(${stampede_plugin_INCLUDE_DIRS})
include_directories(${grab_detector_plugin_INCLUDE_DIRS})
include_directories("./src/3rd_party/qcustomplot")

message(STATUS "bias_plugin_INCLUDE_DIRS='${bias_plugin_INCLUDE_DIRS}'")
message(STATUS "stampede_plugin_INCLUDE_DIRS='${stampede_plugin_INCLUDE_DIRS}'")
message(STATUS "grab_detector_plugin_INCLUDE_DIRS='${grab_detector_plugin_INCLUDE_DIRS}'")

if(with_fc2)
    include_directories(${FlyCapture2_INCLUDE_DIRS})
    include_directories("./src/backend/fc2")
endif()

if(with_dc1394)
    include_directories("./src/backend/dc1394")
    # Add custom libdc1394 
    include_directories("/home/wbd/local/include")
    link_directories("/home/wbd/local/lib")
endif()


# External link libraries
# -----------------------------------------------------------------------------
set(bias_ext_link_LIBS ${OpenCV_LIBS})

if(with_fc2)
    set(bias_ext_link_LIBS ${bias_ext_link_LIBS} ${FlyCapture2_LIBRARIES})
endif()

if(with_dc1394)
    if (WIN32)
        set(bias_ext_link_LIBS ${bias_ext_link_LIBS} dc1394 1394camera setupapi)
    else()
        set(bias_ext_link_LIBS ${bias_ext_link_LIBS} dc1394)
    endif()
endif()
set(bias_ext_link_LIBS ${bias_ext_link_LIBS})


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

add_subdirectory("src/facade")
add_subdirectory("src/utility")
add_subdirectory("src/plugin/base")
add_subdirectory("src/plugin/stampede")
add_subdirectory("src/plugin/grab_detector")
add_subdirectory("src/3rd_party/qcustomplot")

if(with_qt_gui)
    add_subdirectory("src/gui")
endif()

if (with_demos)
    add_subdirectory("src/demo/fly_sorter")
endif()

if (with_tests)
    add_subdirectory("src/test")
endif()

