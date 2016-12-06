CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Bin (
	PROJECT_NAME "fakeconnector"
	PROJECT_DESCRIPTION "This is the fake connector project."
	BIN_DESTINATION bin
)

# install include files
INSTALL( DIRECTORY lua/
	DESTINATION share/fakeconnector/lua
	FILES_MATCHING PATTERN "*.lua"
	PATTERN "*svn*" EXCLUDE
)
