# -*- cmake -*-

# - Find uv library (libuv)
# Find the ev includes and library
# This module defines
#  UV_INCLUDE_DIR, where to find uv.h, etc.
#  UV_LIBRARIES, the libraries needed to use libuv.
#  UV_FOUND, If false, do not try to use libuv.
# also defined, but not for general use are
#  UV_LIBRARY, where to find the libuv library.

FIND_PATH(UV_INCLUDE_DIR uv.h
/usr/local/include
/usr/include
)

SET(UV_NAMES ${UV_NAMES} uv )
FIND_LIBRARY(UV_LIBRARIES
  NAMES ${UV_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set UV_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(UV DEFAULT_MSG UV_LIBRARIES UV_INCLUDE_DIR)

MARK_AS_ADVANCED( UV_INCLUDE_DIR UV_LIBRARIES )
