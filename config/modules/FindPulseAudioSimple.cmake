# -*- cmake -*-

# - Find pulseaudio simple library (libpulsesimple)
# Find the pulseaudio simple includes and library
# This module defines
#  PULSEAUDIO_SIMPLE_INCLUDE_DIR, where to find db.h, etc.
#  PULSEAUDIO_SIMPLE_LIBRARIES, the libraries needed to use libpulsesimple.
#  PULSEAUDIO_SIMPLE_FOUND, If false, do not try to use libpulsesimple.
# also defined, but not for general use are
#  PULSEAUDIO_SIMPLE_LIBRARY, where to find the libpulsesimple library.

FIND_PATH(PULSEAUDIO_SIMPLE_INCLUDE_DIR pulse/simple.h
/usr/local/include
/usr/include
)

SET(PULSEAUDIO_SIMPLE_NAMES ${PULSEAUDIO_SIMPLE_NAMES} pulse-simple )
FIND_LIBRARY(PULSEAUDIO_SIMPLE_LIBRARIES
  NAMES ${PULSEAUDIO_SIMPLE_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set PULSEAUDIO_SIMPLE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PULSEAUDIO_SIMPLE DEFAULT_MSG PULSEAUDIO_SIMPLE_LIBRARIES PULSEAUDIO_SIMPLE_INCLUDE_DIR)

MARK_AS_ADVANCED( PULSEAUDIO_SIMPLE_INCLUDE_DIR PULSEAUDIO_SIMPLE_LIBRARIES )
