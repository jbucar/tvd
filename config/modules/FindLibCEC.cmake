# -*- cmake -*-

# - Find ev library (libcec)
# Find the ev includes and library
# This module defines
#  CEC_INCLUDE_DIR, where to find db.h, etc.
#  CEC_LIBRARIES, the libraries needed to use libcec.
#  CEC_FOUND, If false, do not try to use libcec.
# also defined, but not for general use are
#  CEC_LIBRARY, where to find the libcec library.

FIND_PATH(CEC_INCLUDE_DIR libcec/cec.h
/usr/local/include
/usr/include
)

SET(CEC_NAMES ${CEC_NAMES} cec )
FIND_LIBRARY(CEC_LIBRARIES
  NAMES ${CEC_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CEC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CEC DEFAULT_MSG CEC_LIBRARIES CEC_INCLUDE_DIR)

MARK_AS_ADVANCED( CEC_INCLUDE_DIR CEC_LIBRARIES )
