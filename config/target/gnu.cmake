# GNU generic toolchain macros

# This macro adds the executable for the System
MACRO(add_system_executable PROJECT_NAME PROJECT_NAME_SOURCE_FILES)
    ADD_EXECUTABLE( ${PROJECT_NAME} ${${PROJECT_NAME_SOURCE_FILES}} )
ENDMACRO(add_system_executable)

# Set build targets
macro(SET_BUILD_TARGET build_dir build install)
	add_cmake_build_cmd( ${build_dir} "" "$ENV{MAKE_OPTS}" ${build} )
	add_cmake_build_cmd( ${build_dir} "install" "" ${install} )
endmacro(SET_BUILD_TARGET)

# Set library name
macro(SET_LIBNAME outputvar project_name)
	SET(${outputvar} ${PROJECT_PREFIX}${project_name} )
endmacro(SET_LIBNAME outputvar project_name)

# Set tool name
macro(SET_TOOLNAME outputvar project_name)
	SET(${outputvar} ${project_name})
endmacro(SET_TOOLNAME outputvar project_name)
