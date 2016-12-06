# - Try to find EGL
# Once done, this will define
#
#  EGL_FOUND - system has EGL
#  EGL_INCLUDE_DIR - the EGL include directories
#  EGL_LIBRARIES - link these to use EGL

# Include dir
find_path(EGL_INCLUDE_DIR
	NAMES egl.h
	PATH_SUFFIXES EGL
)

# Finally the library itself
find_library(EGL_LIBRARIES
	NAMES EGL
	PATH_SUFFIXES EGL
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(EGL DEFAULT_MSG EGL_LIBRARIES EGL_INCLUDE_DIR)
mark_as_advanced( EGL_INCLUDE_DIR EGL_LIBRARIES )

