if(WIN32)
  cmake_minimum_required(VERSION 3.4)
else()
  cmake_minimum_required(VERSION 3.1)
endif()

# Fail immediately if not using an out-of-source build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR
    "In-source builds are not supported.  Please create a build directory "
    "separate from the source directory")
endif()

#------------------------------------------------------------------------------#
# Parse version number from fpzip.h
#------------------------------------------------------------------------------#
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/fpzip.h _fpzip_h_contents)
string(REGEX REPLACE ".*#define[ \t]+FPZIP_VERSION_MAJOR[ \t]+([0-9]+).*"
     "\\1" FPZIP_VERSION_MAJOR ${_fpzip_h_contents})
string(REGEX REPLACE ".*#define[ \t]+FPZIP_VERSION_MINOR[ \t]+([0-9]+).*"
    "\\1" FPZIP_VERSION_MINOR ${_fpzip_h_contents})
string(REGEX REPLACE ".*#define[ \t]+FPZIP_VERSION_PATCH[ \t]+([0-9]+).*"
    "\\1" FPZIP_VERSION_PATCH ${_fpzip_h_contents})
set(FPZIP_VERSION
  "${FPZIP_VERSION_MAJOR}.${FPZIP_VERSION_MINOR}.${FPZIP_VERSION_PATCH}")

project(FPZIP VERSION ${FPZIP_VERSION})

#------------------------------------------------------------------------------#
# Some boilerplate to setup nice output directories
#------------------------------------------------------------------------------#
include(GNUInstallDirs)
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/fpzip
  CACHE STRING "Installation CMake subdirectory")

list(INSERT CMAKE_MODULE_PATH 0 "${FPZIP_SOURCE_DIR}/cmake")
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FPZIP_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FPZIP_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FPZIP_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

#------------------------------------------------------------------------------#
# Top level options
#------------------------------------------------------------------------------#

# Windows (Visual Studio) specific options
if(MSVC)
  # Use this to get a usable export library when building a DLL on Windows
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

  # Silence extraneous Visual Studio specific warnings
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS /wd4146 /wd4305)
endif()

# Suggest C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

if(MSVC OR MINGW)
  set(CMAKE_C_STANDARD 90)
endif()

message(STATUS "Compiling with C standard: ${CMAKE_C_STANDARD}")

# Suggest C++98
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 98)
endif()
message(STATUS "Compiling with C++ standard: ${CMAKE_CXX_STANDARD}")

include(CMakeDependentOption)

# Typically you'd always be able to enable shared libraries but default
# configurations with the Cray toolchain will explicitly disable shared lib
# support and only allow static libs.  Making this a cmake_dependent_option
# will ensure that shared library support will be disabled if the system does
# not support it.

# Setup shared library / -fPIC stuff
get_property(SHARED_LIBS_SUPPORTED GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
cmake_dependent_option(BUILD_SHARED_LIBS
  "Whether or not to build shared libraries" ON
  "SHARED_LIBS_SUPPORTED" OFF)

# PIC is always on for shared libs.  This allows it to be selectable for
# static libs.
if(DEFINED FPZIP_ENABLE_PIC)
  set(FPZIP_ENABLE_PIC_DEFAULT ${FPZIP_ENABLE_PIC})
elseif(DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(FPZIP_ENABLE_PIC_DEFAULT ${CMAKE_POSITION_INDEPENDENT_CODE})
else()
  set(FPZIP_ENABLE_PIC_DEFAULT ${SHARED_LIBS_SUPPORTED})
endif()
cmake_dependent_option(FPZIP_ENABLE_PIC
  "Build with Position Independent Code" ${FPZIP_ENABLE_PIC_DEFAULT}
  "SHARED_LIBS_SUPPORTED" OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ${FPZIP_ENABLE_PIC})

# Compile-time options

set(FPZIP_BLOCK_SIZE 4096 CACHE STRING "I/O unit in bytes")
set_property(CACHE FPZIP_BLOCK_SIZE PROPERTY STRINGS "4096")

set(FPZIP_FP FPZIP_FP_FAST CACHE STRING "Floating-point arithmetic")
set_property(CACHE FPZIP_FP PROPERTY STRINGS "FPZIP_FP_FAST;FPZIP_FP_SAFE;FPZIP_FP_EMUL;FPZIP_FP_INT")

option(FPZIP_WITH_REINTERPRET_CAST "Convert to int via reinterpret_cast" OFF)

option(FPZIP_WITH_UNION "Convert to int via union" OFF)

# Handle compile-time macros

list(APPEND fpzip_public_defs FPZIP_FP=${FPZIP_FP})
list(APPEND fpzip_private_defs FPZIP_BLOCK_SIZE=${FPZIP_BLOCK_SIZE})

if((DEFINED FPZIP_INT64) AND (DEFINED FPZIP_INT64_SUFFIX))
  list(APPEND fpzip_public_defs FPZIP_INT64=${FPZIP_INT64})
  list(APPEND fpzip_public_defs FPZIP_INT64_SUFFIX=${FPZIP_INT64_SUFFIX})
endif()

if((DEFINED FPZIP_UINT64) AND (DEFINED FPZIP_UINT64_SUFFIX))
  list(APPEND fpzip_public_defs FPZIP_UINT64=${FPZIP_UINT64})
  list(APPEND fpzip_public_defs FPZIP_UINT64_SUFFIX=${FPZIP_UINT64_SUFFIX})
endif()

# Link libm only if necessary
include(CheckCSourceCompiles)
check_c_source_compiles("#include<math.h>\nfloat f; int main(){sqrt(f);return 0;}" HAVE_MATH)
if(NOT HAVE_MATH)
  set(CMAKE_REQUIRED_LIBRARIES m)
  check_c_source_compiles("#include<math.h>\nfloat f; int main(){sqrt(f);return 0;}" HAVE_LIBM_MATH)
  unset(CMAKE_REQUIRED_LIBRARIES)
  if(NOT HAVE_LIBM_MATH)
    message(FATAL_ERROR "Unable to use C math library functions (with or without -lm)")
  endif()
endif()

#------------------------------------------------------------------------------#
# Add source code
#------------------------------------------------------------------------------#
include(CTest)
if(BUILD_TESTING)
  enable_testing()
endif()

set(FPZIP_LIBRARY_PREFIX "" CACHE STRING
  "Prefix to prepend to the output library name")
mark_as_advanced(FPZIP_LIBRARY_PREFIX)

add_subdirectory(src)

option(BUILD_ALL "Build all subdirectories" OFF)
if(BUILD_ALL)
  set(BUILD_UTILITIES ON CACHE BOOL "Build command-line utilities for fpzip" FORCE)
  set(BUILD_TESTING ON CACHE BOOL "Build tests" FORCE)
endif()

option(BUILD_UTILITIES "Build command-line utilities for fpzip" ON)
if(BUILD_UTILITIES)
  add_subdirectory(utils)
endif()

option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------#
# Header install
#------------------------------------------------------------------------------#
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#------------------------------------------------------------------------------#
# Build type: one of None, Debug, Release, RelWithDebInfo, MinSizeRel
#------------------------------------------------------------------------------#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE Release)
endif()

#------------------------------------------------------------------------------#
# Packaging
#------------------------------------------------------------------------------#

# Add all targets to the build-tree export set
export(TARGETS fpzip NAMESPACE fpzip::
  FILE "${PROJECT_BINARY_DIR}/fpzip-targets.cmake")

configure_file(fpzip-config.cmake.in
  "${PROJECT_BINARY_DIR}/fpzip-config.cmake" @ONLY)
configure_file(fpzip-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/fpzip-config-version.cmake" @ONLY)

# Install the fpzip-config.cmake and fpzip-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/fpzip-config.cmake"
  "${PROJECT_BINARY_DIR}/fpzip-config-version.cmake"
  DESTINATION "${CMAKE_INSTALL_CMAKEDIR}")

# Install the export set for use with the install-tree
install(EXPORT fpzip-targets NAMESPACE fpzip::
  DESTINATION "${CMAKE_INSTALL_CMAKEDIR}")
