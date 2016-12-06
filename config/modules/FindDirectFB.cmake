# - Find DirectFB
# Find native ginga includes and library
# This module defines
#  DIRECTFB_INCLUDE_DIRS, where to find the directfb header files.
#  DIRECTFB_LIBRARIES, the libraries needed to use directfb.
#  DIRECTFB_FOUND, If false, do not try to use directfb.
# also defined, but not for general use are
#  DIRECTFB_LIBRARY, where to find directfb library.

# Find include files
FIND_PATH(DIRECTFB_INCLUDE_DIRS directfb.h PATH_SUFFIXES directfb)
FIND_FILE(DIRECTFB_INCLUDE_VERSION directfb_version.h PATH_SUFFIXES directfb)
FIND_PATH(DIRECTFB_INTERNAL_INCLUDE_DIRS idirectfb.h PATH_SUFFIXES directfb-internal)

# Names for the libraries to find
LIST(APPEND DIRECTFB_NAMES directfb )
LIST(APPEND DIRECTFB_INTERNAL_NAMES direct )

# Check version
IF (DIRECTFB_INCLUDE_VERSION)
	FILE(STRINGS ${DIRECTFB_INCLUDE_VERSION} DIRECTFB_VERSION_MAJOR REGEX ".*DIRECTFB_MAJOR_VERSION.*$" )
	FILE(STRINGS ${DIRECTFB_INCLUDE_VERSION} DIRECTFB_VERSION_MINOR REGEX ".*DIRECTFB_MINOR_VERSION.*$" )
	FILE(STRINGS ${DIRECTFB_INCLUDE_VERSION} DIRECTFB_VERSION_PATCH REGEX ".*DIRECTFB_MICRO_VERSION.*$" )
	STRING(REGEX REPLACE ".*DIRECTFB_MAJOR_VERSION.*\\(([0-9]*)\\).*" "\\1" DIRECTFB_VERSION_MAJOR "${DIRECTFB_VERSION_MAJOR}" )
	STRING(REGEX REPLACE ".*DIRECTFB_MINOR_VERSION.*\\(([0-9]*)\\).*" "\\1" DIRECTFB_VERSION_MINOR "${DIRECTFB_VERSION_MINOR}" )
	STRING(REGEX REPLACE ".*DIRECTFB_MICRO_VERSION.*\\(([0-9]*)\\).*" "\\1" DIRECTFB_VERSION_PATCH "${DIRECTFB_VERSION_PATCH}" )
	SET(DIRECTFB_VERSION "${DIRECTFB_VERSION_MAJOR}.${DIRECTFB_VERSION_MINOR}.${DIRECTFB_VERSION_PATCH}" CACHE STRING "Version of DIRECTFB as computed from directfb_version.h.")
	MARK_AS_ADVANCED(DIRECTFB_VERSION)

	# Add version to the libs names
    foreach( _lib_names ${DIRECTFB_NAMES})
	    list(APPEND DIRECTFB_NAMES "${_lib_names}-${DIRECTFB_VERSION_MAJOR}.${DIRECTFB_VERSION_MINOR}")
    endforeach()

    foreach( _lib_names ${DIRECTFB_INTERNAL_NAMES})
	    list(APPEND DIRECTFB_INTERNAL_NAMES "${_lib_names}-${DIRECTFB_VERSION_MAJOR}.${DIRECTFB_VERSION_MINOR}")
    endforeach()
ENDIF (DIRECTFB_INCLUDE_VERSION)

# Find libraries
FIND_LIBRARY(DIRECTFB_LIBRARY NAMES ${DIRECTFB_NAMES} )
FIND_LIBRARY(DIRECTFB_INTERNAL_LIBRARY NAMES ${DIRECTFB_INTERNAL_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set DIRECTFB_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DIRECTFB DEFAULT_MSG DIRECTFB_LIBRARY DIRECTFB_INCLUDE_DIRS DIRECTFB_INTERNAL_LIBRARY DIRECTFB_INTERNAL_INCLUDE_DIRS )

IF(DIRECTFB_FOUND)
	LIST(APPEND DIRECTFB_INCLUDE_DIRS ${DIRECTFB_INTERNAL_INCLUDE_DIRS} )
	LIST(APPEND DIRECTFB_LIBRARY ${DIRECTFB_INTERNAL_LIBRARY} )

	SET(DIRECTFB_LIBRARIES ${DIRECTFB_LIBRARY})

	# Always set this convenience variable
	SET(DIRECTFB_VERSION_STRING "${DIRECTFB_VERSION}")

	# Here we need to determine if the version we found is acceptable.  We will
	# assume that is unless DIRECTFB_FIND_VERSION_EXACT or DIRECTFB_FIND_VERSION is
	# specified.  The presence of either of these options checks the version
	# string and signals if the version is acceptable or not.
	SET(_directfb_version_acceptable TRUE)
	#
	IF(DIRECTFB_FIND_VERSION_EXACT AND NOT DIRECTFB_VERSION VERSION_EQUAL DirectFB_FIND_VERSION)
		SET(_directfb_version_acceptable FALSE)
	ENDIF()
	#
	IF(DirectFB_FIND_VERSION       AND     DIRECTFB_VERSION VERSION_LESS  DirectFB_FIND_VERSION)
		SET(_directfb_version_acceptable FALSE)
	ENDIF()
	#
	IF(NOT _directfb_version_acceptable)
		SET(_directfb_error_message "Requested DirectFB version ${DirectFB_FIND_VERSION}, but found unacceptable version ${DIRECTFB_VERSION}")
		IF(DirectFB_FIND_REQUIRED)
			message(SEND_ERROR "${_directfb_error_message}")
		ELSEIF(NOT DirectFB_FIND_QUIETLY)
			message(SEND_ERROR "${_directfb_error_message}")
		ENDIF()
		SET(DIRECTFB_FOUND 0)
	ENDIF()
	
ENDIF(DIRECTFB_FOUND)

MARK_AS_ADVANCED(DIRECTFB_LIBRARY DIRECTFB_INCLUDE_DIRS )
