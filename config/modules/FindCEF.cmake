# -*- cmake -*-

# - Find Chromium Embedded Framework
# Find the Chromium Embedded Framework includes and library
# This module defines
#  CEF_INCLUDE_DIR, where to find db.h, etc.
#  CEF_LIBRARIES, the libraries needed to use Chromium Embedded Framework.
#  CEF_FOUND, If false, do not try to use Chromium Embedded Framework.
# also defined, but not for general use are
#  CEF_LIBRARY, where to find the Chromium Embedded Framework library.

# cef.h
FIND_PATH(CEF_INCLUDE_DIR cef/cef.h
  $ENV{CEF_DIR}/include
  /usr/local/include
  /usr/include
  )

# cef_wrapper.h
FIND_PATH(CEF_WRAPPER_INCLUDE_DIR cef/cef_wrapper.h
  $ENV{CEF_DIR}/include
  /usr/local/include
  /usr/include
  )

FIND_LIBRARY(CEF_LIBRARY_RELEASE
  NAMES "libcef.lib"
  PATHS /usr/lib /usr/local/lib $ENV{CEF_DIR}/lib
  )

FIND_LIBRARY(CEF_LIBRARY_DEBUG
  NAMES "libcefd.lib"
  PATHS /usr/lib /usr/local/lib $ENV{CEF_DIR}/lib
  )

FIND_LIBRARY(CEF_WRAPPER_LIBRARY_RELEASE
  NAMES "libcef_dll_wrapper.lib"
  PATHS /usr/lib /usr/local/lib $ENV{CEF_DIR}/lib
  )

FIND_LIBRARY(CEF_WRAPPER_LIBRARY_DEBUG
  NAMES "libcef_dll_wrapperd.lib"
  PATHS /usr/lib /usr/local/lib $ENV{CEF_DIR}/lib
  )

if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  set( CEF_LIBRARIES
    optimized ${CEF_LIBRARY_RELEASE} debug ${CEF_LIBRARY_DEBUG}
    optimized ${CEF_WRAPPER_LIBRARY_RELEASE} debug ${CEF_WRAPPER_LIBRARY_DEBUG}
    )
else()
  set( CEF_LIBRARIES ${CEF_LIBRARY_RELEASE} ${CEF_WRAPPER_LIBRARY_RELEASE} )
endif()
MARK_AS_ADVANCED(CEF_LIBRARY_RELEASE CEF_LIBRARY_DEBUG)
MARK_AS_ADVANCED(CEF_WRAPPER_LIBRARY_RELEASE CEF_WRAPPER_LIBRARY_DEBUG)

IF (CEF_INCLUDE_DIR AND CEF_WRAPPER_INCLUDE_DIR)
    SET(CEF_INCLUDE_DIRS ${CEF_INCLUDE_DIR} ${CEF_WRAPPER_INCLUDE_DIR} )
ENDIF (CEF_INCLUDE_DIR AND CEF_WRAPPER_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CEF_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CEF DEFAULT_MSG CEF_LIBRARIES CEF_INCLUDE_DIR CEF_WRAPPER_INCLUDE_DIR)
mark_as_advanced( CEF_INCLUDE_DIRS CEF_LIBRARIES )

