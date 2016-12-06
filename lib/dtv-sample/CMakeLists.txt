CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Lib (
	PROJECT_NAME "sample"
	PROJECT_DESCRIPTION "This is a sample library." 
	PROJECT_TESTS "test"
)

