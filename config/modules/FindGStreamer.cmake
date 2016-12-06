# - Try to find GStreamer
# Once done this will define
#
#  GSTREAMER_FOUND - system has GStreamer
#  GSTREAMER_INCLUDE_DIR - the GStreamer include directory
#  GSTREAMER_LIBRARIES - the libraries needed to use GStreamer
#  GSTREAMER_DEFINITIONS - Compiler switches required for using GStreamer

# Copyright (c) 2006, Tim Beaulen <tbscope@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
	
FIND_PATH(GSTREAMER_INCLUDE_DIR gst/gst.h
   PATHS
   /usr/include
   /usr/local/include
   /usr/include/gstreamer-1.0
   )

FIND_LIBRARY(GSTREAMER_LIBRARIES
   NAMES gstreamer-1.0
   PATHS /usr/lib /usr/local/lib
   )

FIND_LIBRARY(GSTREAMER_BASE_LIBRARY
   NAMES gstbase-1.0
   PATHS /usr/lib /usr/local/lib
   )
   
FIND_LIBRARY(GSTREAMER_PBUTILS_LIBRARY
  NAMES gstpbutils-1.0
  PATHS /usr/lib /usr/local/lib
  )

FIND_LIBRARY(GSTREAMER_VIDEO_LIBRARY
  NAMES gstvideo-1.0
  PATHS /usr/lib /usr/local/lib
  )
   
FIND_LIBRARY(GSTREAMER_AUDIO_LIBRARY
  NAMES gstaudio-1.0
  PATHS /usr/lib /usr/local/lib
  )

FIND_LIBRARY(GSTREAMER_APP_LIBRARY
  NAMES gstapp-1.0
  PATHS /usr/lib /usr/local/lib
  )

INCLUDE(FindPackageHandleStandardArgs)
SET(GSTREAMER_INCLUDE_DIR ${GSTREAMER_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
SET(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_BASE_LIBRARY} ${GSTREAMER_VIDEO_LIBRARY} ${GSTREAMER_PBUTILS_LIBRARY} ${GSTREAMER_AUDIO_LIBRARY} ${GSTREAMER_APP_LIBRARY} )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GStreamer  DEFAULT_MSG  GSTREAMER_LIBRARIES GSTREAMER_INCLUDE_DIR GSTREAMER_BASE_LIBRARY GSTREAMER_VIDEO_LIBRARY GSTREAMER_PBUTILS_LIBRARY GSTREAMER_AUDIO_LIBRARY)
MARK_AS_ADVANCED(GSTREAMER_INCLUDE_DIR GSTREAMER_LIBRARIES GSTREAMER_BASE_LIBRARY GSTREAMER_VIDEO_LIBRARY GSTREAMER_PBUTILS_LIBRARY GSTREAMER_AUDIO_LIBRARY)
