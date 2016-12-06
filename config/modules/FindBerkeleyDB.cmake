# -*- cmake -*-

# - Find BerkeleyDB
# Find the BerkeleyDB includes and library
# This module defines
#  DB_INCLUDE_DIR, where to find db.h, etc.
#  DB_LIBRARIES, the libraries needed to use BerkeleyDB.
#  DB_FOUND, If false, do not try to use BerkeleyDB.
# also defined, but not for general use are
#  DB_LIBRARY, where to find the BerkeleyDB library.

FIND_PATH(DB_INCLUDE_DIR db.h
$ENV{DB_DIR}/include
/usr/local/include/db4
/usr/local/include
/usr/include/db4
/usr/include
)

SET(DB_NAMES ${DB_NAMES} db libdb53 )
FIND_LIBRARY(DB_LIBRARIES
  NAMES ${DB_NAMES}
  PATHS /usr/lib /usr/local/lib $ENV{DB_DIR}/lib
  )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(DB DEFAULT_MSG DB_LIBRARIES DB_INCLUDE_DIR)
mark_as_advanced( DB_INCLUDE_DIR DB_LIBRARIES )
