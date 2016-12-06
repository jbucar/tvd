CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

if( POLICY CMP0054 )
	cmake_policy(SET CMP0011 OLD)
	cmake_policy(SET CMP0054 OLD)
endif()

INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

IF (NOT COMPILE_PROJECT)
  message(FATAL_ERROR "\n -- \t ERROR: Invalid COMPILE_PROJECT parameter: \"${COMPILE_PROJECT}\"\n")
ENDIF (NOT COMPILE_PROJECT)

# Get all the internal libraries required to build this project
IF(IS_TOOL)
	set( type "tool" )
ELSE()
	set( type "lib" )
ENDIF(IS_TOOL)

CREATE_TARGET( ${type} "${COMPILE_PROJECT}" )

