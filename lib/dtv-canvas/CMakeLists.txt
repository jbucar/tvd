CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Lib (
	PROJECT_NAME "canvas"
	PROJECT_DESCRIPTION "This is the Canvas library project."
	PROJECT_TESTS "test1;test"
)
