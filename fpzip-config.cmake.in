# - Config file for the fpzip package
#
# It defines the following variables
#  FPZIP_INCLUDE_DIRS - include directories for fpzip
#  FPZIP_LIBRARIES    - libraries to link against
#
# And the following imported targets:
#   fpzip::fpzip
#

include("${CMAKE_CURRENT_LIST_DIR}/fpzip-config-version.cmake")

include(FindPackageHandleStandardArgs)
set(${CMAKE_FIND_PACKAGE_NAME}_CONFIG "${CMAKE_CURRENT_LIST_FILE}")
find_package_handle_standard_args(${CMAKE_FIND_PACKAGE_NAME} CONFIG_MODE)

if(NOT TARGET fpzip::fpzip)
  include("${CMAKE_CURRENT_LIST_DIR}/fpzip-targets.cmake")
endif()

set(ZFP_LIBRARIES fpzip::fpzip)
set(ZFP_INCLUDE_DIRS
  $<TARGET_PROPERTY:fpzip::fpzip,INTERFACE_INCLUDE_DIRECTORIES>
)
