CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Bin (
	PROJECT_NAME "wari"
	PROJECT_DESCRIPTION "This is the player project."
	SUBSYSTEM Windows
	BIN_DESTINATION bin
	ICON_NAME "wari.rc"
)

# Add user manual
Add_Doc(
	NAME wari
	SOURCE "doc"
	TEX "wari.tex"
	DESTINATION share/wari
)

# Add build manual
Add_Doc(
	NAME build
	SOURCE "../../doc/build_cmake"
	TEX "build_cmake.tex"
	DESTINATION share/wari
)

IF(MAKE_DOCUMENTATION)
	# hack: generate lua_api.pdf using doc2lua.py
	EXECUTE_PROCESS(
		COMMAND python doc2lua.py ${PROJECT_VERSION}
		WORKING_DIRECTORY $ENV{DEPOT}/lib/dtv-luaz/doc/lua/
	)
	# install lua api documentation
	INSTALL( DIRECTORY ../../lib/dtv-luaz/doc/lua/
		DESTINATION share/wari
		FILES_MATCHING PATTERN "lua_api.pdf"
	)
ENDIF(MAKE_DOCUMENTATION)

# Add dev documentation
Add_Doxygen(
	NAME "dev"
	DESTINATION share/wari
)

# install lua files
INSTALL( DIRECTORY lua/
	DESTINATION share/wari/lua
	FILES_MATCHING PATTERN "*.*"
)

# install fonts files
INSTALL( DIRECTORY fonts/
	DESTINATION share/wari/fonts
	FILES_MATCHING PATTERN "*.ttf"
)

# install imgs
INSTALL( DIRECTORY imgs/
	DESTINATION share/wari/imgs
	FILES_MATCHING PATTERN "*.*"
)
