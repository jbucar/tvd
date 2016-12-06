# Util
set(UTF8_CONVERTER_USE_ICONV 0)
set(UTF8_CONVERTER_USE_EMBED 1)
set(IO_USE_EV 0)
set(IO_USE_EVENT 1)

# Disable DVB for windows
set(DVB_USE_DVB 0)

# Disable zapper logos
set(ZAPPER_LOGOS_USE_TS 0)
set(ZAPPER_LOGOS_USE_NETWORK 0)

# enable windows implementation on dtv-gingawrapper
set(RENDER_USE_WIN 1)

macro(SET_LIBNAME outputvar project_name)
	set( LIBNAME_PREFIX "" )
	IF( "${PLATFORM}" STREQUAL "VS")
		set( LIBNAME_PREFIX lib )
	ENDIF()
	SET(${outputvar} ${LIBNAME_PREFIX}${PROJECT_PREFIX}${project_name})
	IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		SET(${outputvar} ${${outputvar}}d)
	ENDIF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
endmacro(SET_LIBNAME outputvar project_name)

macro(SET_TOOLNAME outputvar project_name)
	SET(${outputvar} ${project_name})
	IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		SET(${outputvar} ${${outputvar}}d)
	ENDIF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
endmacro(SET_TOOLNAME outputvar project_name)

# This macro adds the executable for the System
MACRO(add_system_executable PROJECT_NAME PROJECT_NAME_SOURCE_FILES)
	SetIfEmpty(PRE_BUILD_ICON_NAME "")
	if("${PRE_BUILD_SUBSYSTEM}" STREQUAL "Windows" AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
		ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${${PROJECT_NAME_SOURCE_FILES}} ${PRE_BUILD_ICON_NAME})
	else("${PRE_BUILD_SUBSYSTEM}" STREQUAL "Windows" AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
		ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME_SOURCE_FILES}} ${PRE_BUILD_ICON_NAME})
	endif("${PRE_BUILD_SUBSYSTEM}" STREQUAL "Windows" AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
ENDMACRO(add_system_executable)
