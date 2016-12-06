CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Bin (
	PROJECT_NAME "sample-tool"
	PROJECT_DESCRIPTION "This is a sample tool project."
	BIN_DESTINATION bin
)

Add_Doxygen(
        NAME "dev"
        DESTINATION share/sample-tool
)
