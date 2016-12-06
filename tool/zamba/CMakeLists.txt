CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

SetIfEmpty(ZAMBA_VIDEO_ENABLED 1)
SetIfEmpty(ZAMBA_MUSIC_ENABLED 1)
SetIfEmpty(ZAMBA_APPS_ENABLED 1)

Add_Bin (
	PROJECT_NAME "zamba"
	PROJECT_DESCRIPTION "This is the zapper project."
	PROJECT_SERVICE "ON"
	BIN_DESTINATION bin
)

# install include files
INSTALL( DIRECTORY lua/
	DESTINATION share/zamba/lua
	FILES_MATCHING PATTERN "*.lua"
)
# install include files
INSTALL( DIRECTORY fonts/
	DESTINATION share/zamba/fonts
	FILES_MATCHING PATTERN "*.ttf"
)

# install image files
INSTALL( DIRECTORY imgs/
	DESTINATION share/zamba/imgs
	FILES_MATCHING PATTERN "*.*"
)

# install applications
INSTALL( DIRECTORY apps/
	DESTINATION share/zamba/apps
	FILES_MATCHING PATTERN "*.*"
)

# Add user manual
Add_Doc(
	NAME zamba
	SOURCE "doc/user"
	TEX "zamba.tex"
	DESTINATION share/zamba
)

# Add build manual
Add_Doc(
	NAME build
	SOURCE "../../doc/build_cmake"
	TEX "build_cmake.tex"
	DESTINATION share/zamba
)

# Add dev manual
Add_Doxygen(
	NAME "dev"
	DESTINATION share/zamba
)

IF(MAKE_DOCUMENTATION)
	# hack: generate lua_api.pdf using doc2lua.py
	EXECUTE_PROCESS(
		COMMAND python doc2lua.py ${PROJECT_VERSION}
		WORKING_DIRECTORY $ENV{DEPOT}/lib/dtv-luaz/doc/lua/
	)
	# install lua api documentation
	INSTALL( DIRECTORY ../../lib/dtv-luaz/doc/lua/
		DESTINATION share/zamba
		FILES_MATCHING PATTERN "lua_api.pdf"
	)
ENDIF(MAKE_DOCUMENTATION)
