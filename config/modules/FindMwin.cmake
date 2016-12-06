# - Find MWIN
# Find native ginga includes and library
# This module defines
#  MWIN_INCLUDE_DIRS, where to find the MWIN header files.
#  MWIN_LIBRARIES, the libraries needed to use MWIN.
#  MWIN_FOUND, If false, do not try to use MWIN.
# also defined, but not for general use are
#  MWIN_LIBRARY, where to find MWIN library.

FIND_PATH(MWIN_INCLUDE_DIRS nano-X.h PATH_SUFFIXES microwin )

#FIND_LIBRARY(MWIN_LIBRARY NAMES mwin)

FIND_LIBRARY(MWIN_NANO-X_LIBRARY NAMES nano-X)
LIST(APPEND MWIN_LIBRARY ${MWIN_NANO-X_LIBRARY})

#FIND_LIBRARY(MWIN_ENGINE_LIBRARY NAMES mwengine)
#LIST(APPEND MWIN_LIBRARY ${MWIN_ENGINE_LIBRARY})

#FIND_LIBRARY(MWIN_IMAGES_LIBRARY NAMES mwimages)
#LIST(APPEND MWIN_LIBRARY ${MWIN_IMAGES_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set MWIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MWIN DEFAULT_MSG MWIN_LIBRARY MWIN_INCLUDE_DIRS )


IF(MWIN_FOUND)
	SET(MWIN_LIBRARIES ${MWIN_LIBRARY})

	# Always set this convenience variable
	SET(MWIN_VERSION_STRING "${MWIN_VERSION}")
ENDIF(MWIN_FOUND)

MARK_AS_ADVANCED(MWIN_LIBRARY MWIN_INCLUDE_DIRS )



