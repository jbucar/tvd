# - Try to find Glib-2.0 (with gobject)
# Once done, this will define
#
#  GLIB_FOUND - system has Glib
#  GLIB_INCLUDE_DIRS - the Glib include directories
#  GLIB_LIBRARIES - link these to use Glib

# Glib-related libraries also use a separate config header, which is in lib dir
# Main include dir
find_path(GLIB_INCLUDE_DIR
  NAMES glib.h
  PATHS ${GLIB_PKGCONF_INCLUDE_DIRS} $ENV{GLIB_DIR}/include
  PATH_SUFFIXES glib-2.0
)

find_path(GTHREAD_INCLUDE_DIR
  NAMES glib/gthread.h
  PATHS ${GLIB_PKGCONF_INCLUDE_DIRS} $ENV{GLIB_DIR}/include
  PATH_SUFFIXES glib-2.0
)

find_path(GOBJECT_INCLUDE_DIR
  NAMES gobject/gobject.h
  PATHS ${GLIB_PKGCONF_INCLUDE_DIRS} $ENV{GLIB_DIR}/include
  PATH_SUFFIXES glib-2.0
)

find_path(GlibConfig_INCLUDE_DIR
  NAMES glibconfig.h
  PATH_SUFFIXES lib/glib-2.0/include lib/x86_64-linux-gnu/glib-2.0/include lib/i386-linux-gnu/glib-2.0/include
  PATHS $ENV{GLIB_DIR}
)

set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} ${GOBJECT_INCLUDE_DIR} ${GlibConfig_INCLUDE_DIR} ${GTHREAD_INCLUDE_DIR} )

# Finally the library itself
find_library(GLIB_LIBRARY
  NAMES glib-2.0
  PATHS $ENV{GLIB_DIR}/lib
)

find_library(GTHREAD_LIBRARY
  NAMES gthread-2.0
  PATHS $ENV{GLIB_DIR}/lib
)

find_library(GOBJECT_LIBRARY
  NAMES gobject-2.0
  PATHS $ENV{GLIB_DIR}/lib
)
set(GLIB_LIBRARIES ${GLIB_LIBRARY} ${GOBJECT_LIBRARY} ${GTHREAD_LIBRARY} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GLIB DEFAULT_MSG GLIB_LIBRARIES GLIB_INCLUDE_DIRS)
mark_as_advanced( GLIB_INCLUDE_DIRS GLIB_LIBRARIES )


