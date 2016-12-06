# -*- cmake -*-

# - Find zlib
# Find the zlib includes and library
# This module defines
#  ZLIB_INCLUDE_DIR, where to find db.h, etc.
#  ZLIB_LIBRARIES, the libraries needed to use zlib.
#  ZLIB_FOUND, If false, do not try to use zlib.
# also defined, but not for general use are
#  ZLIB_LIBRARY, where to find the zlib library.

IF("${PLATFORM}" STREQUAL "VS")
	FIND_PATH(ZLIB_INCLUDE_DIR zlib.h
		$ENV{ZLIB_DIR}/include
		/usr/include
	)

	SET(ZLIB_NAMES ${ZLIB_NAMES} zlibwapi )
	FIND_LIBRARY(ZLIB_LIBRARIES
		NAMES ${ZLIB_NAMES}
		PATHS /usr/lib /usr/local/lib $ENV{ZLIB_DIR}/lib
	)

	ADD_DEFINITIONS(-DZLIB_WINAPI)
ELSE()
	find_path( ZLIB_INCLUDE_DIR zlib.h )
	SET(ZLIB_LIBRARIES "libz.dylib")
ENDIF()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ZLIB DEFAULT_MSG ZLIB_LIBRARIES ZLIB_INCLUDE_DIR)
mark_as_advanced( ZLIB_INCLUDE_DIR ZLIB_LIBRARIES)

