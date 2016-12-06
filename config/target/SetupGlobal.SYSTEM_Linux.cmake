# Set warning level
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread") # add "-pthread" here as a workaround for a bug in gtest (https://groups.google.com/group/googletestframework/browse_thread/thread/d00d069043a2f22b#
if ("${PLATFORM_BINARY_TYPE}" STREQUAL "lib" OR BUILD_SHARED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

IF (NOT "${PLATFORM}" STREQUAL "ANDROID")
	# Set std libraries
	set(CMAKE_CXX_STANDARD_LIBRARIES "-lrt")
	set(UTIL_BASE_USE_LINUX 1)
ENDIF()

setifempty(STRIP_TOOL strip )

# Generic toolchain setup
include(target/gnu)

