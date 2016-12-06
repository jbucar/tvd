# -*- cmake -*-

# - Find dvbcsa library (libdvbcsa)
# Find the dvbcsa includes and library
# This module defines
#  DVBCSA_INCLUDE_DIR, where to find db.h, etc.
#  DVBCSA_LIBRARIES, the libraries needed to use libdvbcsa.
#  DVBCSA_FOUND, If false, do not try to use libdvbcsa.
# also defined, but not for general use are
#  DVBCSA_LIBRARY, where to find the libdvbcsa library.

FIND_PATH(DVBCSA_INCLUDE_DIR dvbcsa/dvbcsa.h
/usr/local/include
/usr/include
)

SET(DVBCSA_NAMES ${DVBCSA_NAMES} dvbcsa )
FIND_LIBRARY(DVBCSA_LIBRARIES
  NAMES ${DVBCSA_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DVBCSA_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(DVBCSA DEFAULT_MSG DVBCSA_LIBRARIES DVBCSA_INCLUDE_DIR)

MARK_AS_ADVANCED( DVBCSA_INCLUDE_DIR DVBCSA_LIBRARIES )
