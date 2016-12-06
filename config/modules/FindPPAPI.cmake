# -*- cmake -*-

# - Find ppapi library (libppapi)
# Find the ppapi includes and library
# This module defines
#  PPAPI_INCLUDE_DIR, where to find db.h, etc.
#  PPAPI_LIBRARIES, the libraries needed to use libppapi.
#  PPAPI_FOUND, If false, do not try to use libppapi.
# also defined, but not for general use are
#  PPAPI_LIBRARY, where to find the libppapi library.

if ( NOT "$ENV{PPAPI_BASE}" STREQUAL "" )
  FILE(TO_CMAKE_PATH $ENV{PPAPI_BASE} PPAPI_BASE)
endif ( NOT "$ENV{PPAPI_BASE}" STREQUAL "" )
FIND_PATH( PPAPI_INCLUDE_DIR "ppapi"
  /usr/local/include
  /usr/include
  ${PPAPI_BASE}/include
)

# Library
FIND_LIBRARY( PPAPI_LIBRARIES
  NAMES libppapi.a
  PATHS /usr/lib /usr/local/lib ${PPAPI_BASE}/lib
)


include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set PPAPI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PPAPI DEFAULT_MSG PPAPI_LIBRARIES PPAPI_INCLUDE_DIR)
mark_as_advanced( PPAPI_INCLUDE_DIR PPAPI_LIBRARIES )
