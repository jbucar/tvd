# - Try to find OpenGLES2
# Once done, this will define
#
#  OPENGLES_FOUND - system has OpenGLES2
#  OPENGLES_INCLUDE_DIR - the OpenGLES2 include directories
#  OPENGLES_LIBRARIES - link these to use OpenGLES2

# Include dir
IF(NOT IOS)
	find_path(OPENGLES_INCLUDE_DIR
		NAMES gl2.h
		PATH_SUFFIXES GLES2
	)

	# Finally the library itself
	find_library(OPENGLES_LIBRARIES
		NAMES GLESv2
		PATH_SUFFIXES GLES2
	)
ELSE()
	set( OPENGLES_INCLUDE_DIR "${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/OpenGLES.framework/Headers" )
	set( OPENGLES_LIBRARIES "-framework OpenGLES" )
ENDIF(NOT IOS)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OPENGLES DEFAULT_MSG OPENGLES_LIBRARIES OPENGLES_INCLUDE_DIR)
mark_as_advanced( OPENGLES_INCLUDE_DIR OPENGLES_LIBRARIES )

