# - Try to find FlyCapture2 
# 
# Once done this will define
#
#  FlyCapture2_FOUND         - System has FlyCapture2
#  FlyCapture2_INCLUDE_DIRS  - The FlyCapture2 include directories
#  FlyCapture22_LIBRARIES    - The libraries needed to use FlyCapture2
#
# ------------------------------------------------------------------------------

if (WIN32)
    set(typical_fc2_dir "C:/Program Files/Point Grey Research/FlyCapture2")
    set(typical_fc2_lib_dir "${typical_fc2_dir}/lib64/C")
    set(typical_fc2_inc_dir "${typical_fc2_dir}/include/C")
else()
    set(typical_fc2_dir "/usr")
    set(typical_fc2_lib_dir "${typical_fc2_dir}/lib")
    set(typical_fc2_inc_dir "${typical_fc2_dir}/include/flycapture/C")
endif()

message(STATUS "${typical_fc2_inc_dir}")

message(STATUS "finding include dir")
find_path(
    FlyCapture2_INCLUDE_DIR 
    "FlyCapture2_C.h"
    HINTS ${typical_fc2_inc_dir}
    )
message(STATUS "FlyCapture2_INCLUDE_DIR: " ${FlyCapture2_INCLUDE_DIR})

if(WIN32)
    message(STATUS "finding library")
    find_library(
        FlyCapture2_LIBRARY 
        NAMES "FlyCapture2_C_v100.lib"
        HINTS ${typical_fc2_lib_dir} 
        )
else() 
    message(STATUS "finding library")
    find_library(
        FlyCapture2_LIBRARY 
        NAMES "libflycapture-c.so"
        HINTS ${typical_fc2_lib_dir} 
        )

endif()


set(FlyCapture2_LIBRARIES ${FlyCapture2_LIBRARY} )
set(FlyCapture2_INCLUDE_DIRS ${FlyCapture2_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FlyCapture2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
    FlyCapture2  DEFAULT_MSG
    FlyCapture2_LIBRARY 
    FlyCapture2_INCLUDE_DIR
    )

mark_as_advanced(FlyCapture2_INCLUDE_DIR FlyCapture2_LIBRARY )

