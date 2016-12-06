# - Find MINIGUI
# Find native ginga includes and library
# This module defines
#  MINIGUI_INCLUDE_DIRS, where to find the MINIGUI header files.
#  MINIGUI_LIBRARIES, the libraries needed to use MINIGUI.
#  MINIGUI_FOUND, If false, do not try to use MINIGUI.
# also defined, but not for general use are
#  MINIGUI_LIBRARY, where to find MINIGUI library.

FIND_PATH(MINIGUI_INCLUDE_DIRS minigui/minigui.h )

LIST(APPEND MINIGUI_NAMES minigui )
FIND_LIBRARY(MINIGUI_LIBRARY NAMES ${MINIGUI_NAMES} PATH_SUFFIXES minigui )

# handle the QUIETLY and REQUIRED arguments and set MINIGUI_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MINIGUI DEFAULT_MSG MINIGUI_LIBRARY MINIGUI_INCLUDE_DIRS )

IF(MINIGUI_FOUND)
	SET(MINIGUI_LIBRARIES ${MINIGUI_LIBRARY})

	# Always set this convenience variable
	SET(MINIGUI_VERSION_STRING "${MINIGUI_VERSION}")
ENDIF(MINIGUI_FOUND)

MARK_AS_ADVANCED(MINIGUI_LIBRARY MINIGUI_INCLUDE_DIRS )
