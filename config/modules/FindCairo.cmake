# - Try to find Cairo
# Once done, this will define
#
#  CAIRO_FOUND - system has Cairo
#  CAIRO_INCLUDE_DIRS - the Cairo include directories
#  CAIRO_LIBRARIES - link these to use Cairo

# Include dir
find_path(CAIRO_INCLUDE_DIR
  NAMES cairo.h
  PATH_SUFFIXES cairo
  PATHS $ENV{CAIRO_DIR}/include
)

# Finally the library itself
find_library(CAIRO_LIBRARIES
  NAMES cairo
  PATHS $ENV{CAIRO_DIR}/lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CAIRO DEFAULT_MSG CAIRO_LIBRARIES CAIRO_INCLUDE_DIR)
mark_as_advanced( CAIRO_INCLUDE_DIR CAIRO_LIBRARIES )

