# -*- cmake -*-

# - Find ev library (libev)
# Find the ev includes and library
# This module defines
#  EV_INCLUDE_DIR, where to find db.h, etc.
#  EV_LIBRARIES, the libraries needed to use libev.
#  EV_FOUND, If false, do not try to use libev.
# also defined, but not for general use are
#  EV_LIBRARY, where to find the libev library.

FIND_PATH(EV_INCLUDE_DIR ev.h
/usr/local/include
/usr/include
)

SET(EV_NAMES ${EV_NAMES} ev )
FIND_LIBRARY(EV_LIBRARIES
  NAMES ${EV_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set EV_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(EV DEFAULT_MSG EV_LIBRARIES EV_INCLUDE_DIR)

MARK_AS_ADVANCED( EV_INCLUDE_DIR EV_LIBRARIES )
