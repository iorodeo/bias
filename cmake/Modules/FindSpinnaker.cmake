# - Try to find Spinnaker 
# 
# Once done this will define
#
#  Spinnaker_FOUND         - System has LibXml2
#  Spinnaker_INCLUDE_DIRS  - The Spinnaker include directories
#  Spinnaker_LIBRARIES     - The libraries needed to use Spinnaker 
#
# ------------------------------------------------------------------------------

if (WIN32)
    message(STATUS "FindSpinnaker system is WIN232")
    set(typical_spin_dir "C:/Program Files/Point Grey Research/Spinnaker")
    set(typical_spin_lib_dir "${typical_spin_dir}/lib64/vs2013")
    set(typical_spin_inc_dir "${typical_spin_dir}/include/spinc")
else()
    message(STATUS "FindSpinnaker system is not WIN232")
    #set(typical_spin_dir "/usr")
    #set(typical_spin_lib_dir "${typical_spin_dir}/lib")
    #set(typical_spin_inc_dir "${typical_spin_dir}/include/flycapture/C")
endif()

message(STATUS "${typical_spin_inc_dir}")

message(STATUS "finding include dir")
find_path(
    Spinnaker_INCLUDE_DIR 
    "SpinnakerC.h"
    HINTS ${typical_spin_inc_dir}
    )
message(STATUS "Spinnaker_INCLUDE_DIR: " ${Spinnaker_INCLUDE_DIR})

if(WIN32)
    message(STATUS "finding library")
    find_library(
        Spinnaker_LIBRARY 
        NAMES "SpinnakerC_v120.lib"
        HINTS ${typical_spin_lib_dir} 
        )
else() 
    #message(STATUS "finding library")
    #find_library(
    #    Spinnaker_LIBRARY 
    #    NAMES "libflycapture-c.so"
    #    HINTS ${typical_spin_lib_dir} 
    #    )
endif()


set(Spinnaker_LIBRARIES ${Spinnaker_LIBRARY} )
set(Spinnaker_INCLUDE_DIRS ${Spinnaker_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set Spinnaker_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
    Spinnaker  DEFAULT_MSG
    Spinnaker_LIBRARY 
    Spinnaker_INCLUDE_DIR
    )

mark_as_advanced(Spinnaker_INCLUDE_DIR Spinnaker_LIBRARY )

