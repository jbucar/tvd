# - Try to find OpenGL
# Once done, this will define
#
#  GL_FOUND - system has OpenGL
#  GL_INCLUDE_DIR - the OpenGL include directories
#  GL_LIBRARIES - link these to use OpenGL

# Include dir
find_path(GL_INCLUDE_DIR
	NAMES gl.h
	PATH_SUFFIXES GL
)

# Finally the library itself
find_library(GL_LIBRARIES
	NAMES GL opengl32
	PATH_SUFFIXES GL
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GL_FOUND to TRUE if all listed variables are TRUE
find_package_handle_standard_args(GL DEFAULT_MSG GL_LIBRARIES GL_INCLUDE_DIR)
mark_as_advanced( GL_INCLUDE_DIR GL_LIBRARIES )
