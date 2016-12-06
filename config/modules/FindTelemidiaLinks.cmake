# - Find TelemidiaLinks
# Find native ginga includes and library
# This module defines
#  TELEMIDIALINKS_INCLUDE_DIRS, where to find the telemidialinks header files.
#  TELEMIDIALINKS_LIBRARIES, the libraries needed to use telemidialinks.
#  TELEMIDIALINKS_FOUND, If false, do not try to use telemidialinks.
# also defined, but not for general use are
#  TELEMIDIALINKS_LIBRARY, where to find telemidialinks library.

FIND_PATH(TELEMIDIALINKS_INCLUDE_DIRS telemidialinks.h PATH_SUFFIXES ginga/player )

LIST(APPEND TELEMIDIALINKS_NAMES telemidialinks )
FIND_LIBRARY(TELEMIDIALINKS_LIBRARY NAMES ${TELEMIDIALINKS_NAMES} PATH_SUFFIXES ginga )

# handle the QUIETLY and REQUIRED arguments and set TELEMIDIALINKS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TELEMIDIALINKS DEFAULT_MSG TELEMIDIALINKS_LIBRARY TELEMIDIALINKS_INCLUDE_DIRS )

IF(TELEMIDIALINKS_FOUND)
	SET(TELEMIDIALINKS_LIBRARIES ${TELEMIDIALINKS_LIBRARY})

	# Always set this convenience variable
	SET(TELEMIDIALINKS_VERSION_STRING "${TELEMIDIALINKS_VERSION}")
ENDIF(TELEMIDIALINKS_FOUND)

MARK_AS_ADVANCED(TELEMIDIALINKS_LIBRARY TELEMIDIALINKS_INCLUDE_DIRS )
