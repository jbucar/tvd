# -*- cmake -*-

# - Find breakpad library
# Find the breakpad includes and library
# This module defines
#  BREAKPAD_INCLUDE_DIR, where to find breakpad.h, etc.
#  BREAKPAD_LIBRARIES, the libraries needed to use libbreakpad.
#  BREAKPAD_FOUND, If false, do not try to use libbreakpad.
# also defined, but not for general use are
#  BREAKPAD_LIBRARY, where to find the libbreakpad library.

# -I/usr/include/breakpad
# client/linux/handler/exception_handler.h

FIND_PATH(BREAKPAD_INCLUDE_DIR client/linux/handler/exception_handler.h
/usr/local/include
/usr/include
PATH_SUFFIXES breakpad
)

FIND_LIBRARY(BREAKPAD_LIBRARY
  NAMES breakpad
  PATHS /usr/lib /usr/local/lib
)

FIND_LIBRARY(BREAKPAD_CLIENT_LIBRARY
  NAMES breakpad_client
  PATHS /usr/lib /usr/local/lib
)

set(BREAKPAD_LIBRARIES ${BREAKPAD_LIBRARY} ${BREAKPAD_CLIENT_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set BREAKPAD_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(BREAKPAD DEFAULT_MSG BREAKPAD_LIBRARIES BREAKPAD_INCLUDE_DIR)

MARK_AS_ADVANCED( BREAKPAD_INCLUDE_DIR BREAKPAD_LIBRARIES )
