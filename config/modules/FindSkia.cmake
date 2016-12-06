# -*- cmake -*-

# - Find Skia
# Find the Skia includes and library
# This module defines
#  SKIA_INCLUDE_DIR, where to find db.h, etc.
#  SKIA_LIBRARY, the libraries needed to use BerkeleyDB.
#  SKIA_FOUND, If false, do not try to use BerkeleyDB.
# also defined, but not for general use are
#  SKIA_LIBRARY, where to find the BerkeleyDB library.

FIND_PATH( SKIA_CONFIG_INCLUDE_DIR "skia/config/SkUserConfig.h"
$ENV{SKIA_DIR}/include
/usr/local/include
/usr/include
$ENV{SKIA_DIR}/include
)

FIND_PATH( SKIA_CORE_INCLUDE_DIR "skia/core/SkCanvas.h"
$ENV{SKIA_DIR}/include
/usr/local/include
/usr/include
$ENV{SKIA_DIR}/include
)

SET( SKIA_INCLUDE_DIRS "${SKIA_CONFIG_INCLUDE_DIR}/skia/config" "${SKIA_CORE_INCLUDE_DIR}/skia/core" "${SKIA_CORE_INCLUDE_DIR}/skia/images" "${SKIA_CORE_INCLUDE_DIR}/skia/lazy")

# Library
SET( SKIA_RELEASE_NAMES libskia skia )
FIND_LIBRARY( SKIA_LIBRARY_RELEASE
  NAMES ${SKIA_RELEASE_NAMES}
  PATHS /usr/lib /usr/local/lib $ENV{SKIA_DIR}/lib
  )

Find_Package( PNG REQUIRED )
Find_Package( GL REQUIRED )
if(NOT MSVC)
  Find_Package( Freetype REQUIRED )
  Find_Package( JPEG REQUIRED )
endif()

set( SKIA_DEFINITIONS "-DSK_RELEASE" )
set( SKIA_LIBRARIES ${SKIA_LIBRARY_RELEASE} ${PNG_LIBRARIES} ${FREETYPE_LIBRARIES} ${JPEG_LIBRARIES} ${GL_LIBRARIES})
set( SKIA_INCLUDE_DIRS ${SKIA_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS} ${GL_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SKIA_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Skia DEFAULT_MSG SKIA_LIBRARIES SKIA_INCLUDE_DIRS)
mark_as_advanced( SKIA_INCLUDE_DIRS SKIA_LIBRARIES )