CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Bin (
	PROJECT_NAME "ginga"
	PROJECT_DESCRIPTION "Lifia implementation of Ginga reference specification for south american digital TV"
	SUBSYSTEM Windows
	BIN_DESTINATION bin
	ICON_NAME "ginga.rc"
	LICENSE_FILE "${PROJECT_SOURCE_DIR}/ginga.lgpl.txt"
)

INSTALL( DIRECTORY examples/
	DESTINATION share/ginga/examples
	FILES_MATCHING PATTERN "*.*"
)

INSTALL( DIRECTORY fonts/
	DESTINATION share/ginga/fonts
	FILES_MATCHING PATTERN "*.ttf"
		PATTERN "*svn*" EXCLUDE
)

# install icon files
INSTALL( DIRECTORY res/
	DESTINATION share/ginga/res
	FILES_MATCHING PATTERN "ginga.ico"
)

# Add user manual
# Add_Doc(
# 	NAME ginga
# 	SOURCE "doc"
# 	TEX "ginga.tex"
# 	DESTINATION share/ginga
# )

# Add build manual
Add_Doc(
	NAME build
	SOURCE "../../doc/build_cmake"
	TEX "build_cmake.tex"
	DESTINATION share/ginga
)

Add_Doxygen(
	NAME "dev"
	DESTINATION share/ginga
)

