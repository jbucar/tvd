# - Try to find Pango
# Once done, this will define
#
#  PANGO_FOUND - system has Pango
#  PANGO_INCLUDE_DIRS - the Pango include directories
#  PANGO_LIBRARIES - link these to use Pango

# Includes
find_path(PANGO_INCLUDE_DIRS
  NAMES pango/pango.h
  PATH_SUFFIXES pango-1.0
  PATHS $ENV{PANGO_DIR}/include
)

# Libraries
find_library(PANGO_LIBRARY
  NAMES pango-1.0
  PATHS $ENV{PANGO_DIR}/lib
)

find_library(PANGOCAIRO_LIBRARY
  NAMES pangocairo-1.0
  PATHS $ENV{PANGO_DIR}/lib
)

set(PANGO_LIBRARIES ${PANGO_LIBRARY} ${PANGOCAIRO_LIBRARY} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PANGO DEFAULT_MSG PANGO_LIBRARIES PANGO_INCLUDE_DIRS)
mark_as_advanced( PANGO_INCLUDE_DIRS PANGO_LIBRARIES )
