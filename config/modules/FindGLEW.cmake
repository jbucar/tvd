# - Try to find GLEW
# Once done, this will define
#
#  GLEW_FOUND - system has GLEW
#  GLEW_INCLUDE_DIR - GLEW include directories
#  GLEW_LIBRARIES - link these to use GLEW

# Include dir
find_path(GLEW_INCLUDE_DIR
	NAMES glew.h
	PATH_SUFFIXES GL
)

# Finally the library itself
find_library(GLEW_LIBRARIES
	NAMES GLEW glew32
	PATH_SUFFIXES GL
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GLEW_FOUND to TRUE if all listed variables are TRUE
find_package_handle_standard_args(GLEW DEFAULT_MSG GLEW_LIBRARIES GLEW_INCLUDE_DIR)
mark_as_advanced( GLEW_INCLUDE_DIR GLEW_LIBRARIES )
