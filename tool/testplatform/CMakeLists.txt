CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Bin (
	PROJECT_NAME "testplatform"
	PROJECT_DESCRIPTION "This is the test platform project."
	BIN_DESTINATION bin
)

# Add user manual
Add_Doc(
	NAME testplatform
	SOURCE "doc"
	TEX "testplatform.tex"
	DESTINATION share/testplatform
)

# Add build manual
Add_Doc(
	NAME build
	SOURCE "../../doc/build_cmake"
	TEX "build_cmake.tex"
	DESTINATION share/testplatform
)

# install fonts
INSTALL( DIRECTORY fonts/
	DESTINATION share/testplatform/fonts
	FILES_MATCHING PATTERN "*.ttf"
)

# install image files
INSTALL( DIRECTORY imgs/
	DESTINATION share/testplatform/imgs
	FILES_MATCHING PATTERN "*.*"
)

# install media files
INSTALL( DIRECTORY media/
	DESTINATION share/testplatform/media
	FILES_MATCHING PATTERN "*.*"
)

# install config files
INSTALL( DIRECTORY config/
	DESTINATION share/testplatform/config
	FILES_MATCHING PATTERN "*.xml"
)