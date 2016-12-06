# Find LibEvent (a cross event library)
# This module defines
# LIBEVENT_INCLUDE_DIR, where to find LibEvent headers
# LIBEVENT_LIB, LibEvent libraries
# LIBEVENT_FOUND, If false, do not try to use libevent

FIND_PATH(LIBEVENT_INCLUDE_DIR event2/event.h
	/usr/local/include
	/usr/include
	$ENV{LIBEVENT_DIR}/include
)

FIND_LIBRARY(LIBEVENT_LIBRARIES
	NAMES event_core libevent_core.lib
	PATHS /usr/lib /usr/local/lib $ENV{LIBEVENT_DIR}/lib
)

IF (NOT WIN32)
	FIND_LIBRARY(LIBEVENT_PTHREAD_LIB
		NAMES event_pthreads
		PATHS /usr/lib /usr/local/lib
	)
	SET(LIBEVENT_LIBRARIES ${LIBEVENT_LIBRARIES} ${LIBEVENT_PTHREAD_LIB})
ENDIF (NOT WIN32)

IF("${PLATFORM}" STREQUAL "VS")
	ADD_DEFINITIONS(-DWIN32)
ENDIF("${PLATFORM}" STREQUAL "VS")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBEVENT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBEVENT DEFAULT_MSG LIBEVENT_LIBRARIES LIBEVENT_INCLUDE_DIR)

MARK_AS_ADVANCED( LIBEVENT_INCLUDE_DIR LIBEVENT_LIBRARIES )
