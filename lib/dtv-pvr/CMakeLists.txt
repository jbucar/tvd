CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

# Filter mobile channels
SetIfEmpty( PVR_FILTER_MOBILE_CHANNELS 0 )

Add_Lib (
	PROJECT_NAME "pvr"
	PROJECT_DESCRIPTION "This is a pvr library."
	PROJECT_TESTS "test"
)

