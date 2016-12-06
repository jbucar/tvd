# - Find curl
# Find the native MINIZIP headers and libraries.
#
#  MINIZIP_INCLUDE_DIRS - where to find curl/curl.h, etc.
#  MINIZIP_LIBRARIES    - List of libraries when using curl.
#  MINIZIP_FOUND        - True if curl found.

#=============================================================================
# Copyright 2006-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# Look for the header file.
FIND_PATH(MINIZIP_INCLUDE_DIR NAMES minizip/zip.h
	PATHS $ENV{MINIZIP_DIR}/include
)
MARK_AS_ADVANCED(MINIZIP_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(MINIZIP_LIBRARIES NAMES
    minizip
	PATHS $ENV{MINIZIP_DIR}/lib $ENV{MINIZIP_DIR}/bin
)
MARK_AS_ADVANCED(MINIZIP_LIBRARIES)

# handle the QUIETLY and REQUIRED arguments and set MINIZIP_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MINIZIP DEFAULT_MSG MINIZIP_LIBRARIES MINIZIP_INCLUDE_DIR)

