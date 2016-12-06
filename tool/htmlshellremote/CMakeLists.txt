CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

set(CMAKE_CXX_STANDARD 11)

Add_Bin (
	PROJECT_NAME "htmlshellremote"
	PROJECT_DESCRIPTION "HtmlShell remote that export the browser api throw a protobuf message"
	BIN_TYPE "lib"
)

INCLUDE($ENV{DEPOT}/config/LoadableModule.cmake)
# NodeJS addon used in tests (not linked to htmlshellremote)
Build_Internal_Lib("htmlshellmock")

# Run GenerateNodeModule.cmake after build
# Generate the htmlshellremote NodeJS addon (included it tests).
ADD_CUSTOM_COMMAND(
	TARGET htmlshellmock
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
		-DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
		-DBIN_DIR=${CMAKE_CURRENT_BINARY_DIR}
		-P ${CMAKE_CURRENT_SOURCE_DIR}/GenerateNodeModule.cmake)

# Install the generated htmlshellremote NodeJS addon in $INSTALL_DIR/share/htmlshellremote 
# Do not install tests
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/htmlshellremote/
	DESTINATION share/htmlshellremote
	FILES_MATCHING PATTERN "*"
	PATTERN "test" EXCLUDE)
