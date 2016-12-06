# - Try to find FreeImage
# Once done, this will define
#
#  FREEIMAGE_FOUND - system has FreeImage
#  FREEIMAGE_INCLUDE_DIR - the FreeImage include directories
#  FREEIMAGE_LIBRARIES - link these to use FreeImage

# Include dir
find_path(FREEIMAGE_INCLUDE_DIR
	NAMES FreeImage.h
)

# Finally the library itself
find_library(FREEIMAGE_LIBRARIES
	NAMES freeimage FreeImage.lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FREEIMAGE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FREEIMAGE DEFAULT_MSG FREEIMAGE_LIBRARIES FREEIMAGE_INCLUDE_DIR)
mark_as_advanced( FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARIES )

