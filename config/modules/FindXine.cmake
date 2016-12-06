# - Find Xine
# Find native ginga includes and library
# This module defines
#  XINE_INCLUDE_DIRS, where to find the xine header files.
#  XINE_LIBRARIES, the libraries needed to use xine.
#  XINE_FOUND, If false, do not try to use xine.
# also defined, but not for general use are
#  XINE_LIBRARY, where to find xine library.

FIND_PATH(XINE_INCLUDE_DIRS xine.h )

LIST(APPEND XINE_NAMES xine )
FIND_LIBRARY(XINE_LIBRARY NAMES ${XINE_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set XINE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(XINE DEFAULT_MSG XINE_LIBRARY XINE_INCLUDE_DIRS )

SET(HAVE_XINE 0)
IF(XINE_FOUND)
	SET(HAVE_XINE 1)
	SET(XINE_LIBRARIES ${XINE_LIBRARY})

	FILE(STRINGS ${XINE_INCLUDE_DIRS}/xine.h XINE_VERSION_MAJOR REGEX ".*XINE_MAJOR_VERSION.*$" )
	FILE(STRINGS ${XINE_INCLUDE_DIRS}/xine.h XINE_VERSION_MINOR REGEX ".*XINE_MINOR_VERSION.*$" )
	FILE(STRINGS ${XINE_INCLUDE_DIRS}/xine.h XINE_VERSION_PATCH REGEX ".*XINE_SUB_VERSION.*$" )
	STRING(REGEX REPLACE ".*XINE_MAJOR_VERSION.*([0-9]+).*" "\\1" XINE_VERSION_MAJOR "${XINE_VERSION_MAJOR}" )
	STRING(REGEX REPLACE ".*XINE_MINOR_VERSION.*([0-9]+).*" "\\1" XINE_VERSION_MINOR "${XINE_VERSION_MINOR}" )
	STRING(REGEX REPLACE ".*XINE_MICRO_VERSION.*([0-9]+).*" "\\1" XINE_VERSION_PATCH "${XINE_VERSION_PATCH}" )
	SET(XINE_VERSION "${XINE_VERSION_MAJOR}.${XINE_VERSION_MINOR}.${XINE_VERSION_PATCH}" CACHE STRING "Version of XINE as computed from xine_version.h.")
	MARK_AS_ADVANCED(XINE_VERSION)

	# Always set this convenience variable
	SET(XINE_VERSION_STRING "${XINE_VERSION}")

	# Here we need to determine if the version we found is acceptable.  We will
	# assume that is unless XINE_FIND_VERSION_EXACT or XINE_FIND_VERSION is
	# specified.  The presence of either of these options checks the version
	# string and signals if the version is acceptable or not.
	SET(_xine_version_acceptable TRUE)
	#
	IF(XINE_FIND_VERSION_EXACT AND NOT XINE_VERSION VERSION_EQUAL XINE_FIND_VERSION)
		SET(_xine_version_acceptable FALSE)
	ENDIF()
	#
	IF(XINE_FIND_VERSION       AND     XINE_VERSION VERSION_LESS  XINE_FIND_VERSION)
		SET(_xine_version_acceptable FALSE)
	ENDIF()
	#
	IF(NOT _xine_version_acceptable)
		SET(_xine_error_message "Requested Xine version ${XINE_FIND_VERSION}, but found unacceptable version ${XINE_VERSION}")
		IF(XINE_FIND_REQUIRED)
			message("${_xine_error_message}")
		ELSEIF(NOT XINE_FIND_QUIETLY)
			message("${_xine_error_message}")
		ENDIF()
	ENDIF()
	

ENDIF(XINE_FOUND)

MARK_AS_ADVANCED(XINE_LIBRARY XINE_INCLUDE_DIRS )
