CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Lib (
	PROJECT_NAME "htmlshellsdk"
	PROJECT_DESCRIPTION "This is the sdk used by htmlshell extensions." 
	PROJECT_TESTS "test"
)
