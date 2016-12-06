CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# NOTE: This file is a HACK for supporting building of internal loadable libraries.
# Use with:
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
#	set(BUILD_SHARED 0)

INCLUDE($ENV{DEPOT}/config/Impl.cmake)

# HACK: Override BUILD_PROJECT_DEPS in $DEPOT/config/Impl.cmake to create a loadable .so linked staticaly
MACRO( BUILD_PROJECT_DEPS type name src out )
	# Add direct internal dependencies (only cflags)
	get_internal_components( ${name}_info_INTERNAL_COMPONENTS internals )
	add_dependents_list( internals 1 0 ${out} )

	# Add direct external dependencies (only cflags)
	add_dependents_list( ${name}_info_EXTERNAL_COMPONENTS 1 0 ${out} )

	# Get all internal dependencies, in order
	GET_PROJECT_BUILD_ORDER( ${type} ${name} ${src} build_list 0 )

	# Reverse list
	IF (build_list)
		LIST( REVERSE build_list )
	ENDIF (build_list)

	# Add all internal components (only libs)
	get_internal_components( build_list comp_internals )
	add_dependents_list( comp_internals 0 1 ${out} )

	# Join all external components
	FOREACH( dep ${build_list} )
		LIST( APPEND externals ${${dep}_info_EXTERNAL_COMPONENTS} )
	ENDFOREACH( dep ${build_list} )
	LIST( APPEND externals ${${name}_info_EXTERNAL_COMPONENTS} )
	CLEAN_LIST( externals )

	# Add all externals (only libs)
	add_dependents_list( externals 0 1 ${out} )
ENDMACRO( BUILD_PROJECT_DEPS type name out )

# This macro allow libs/tools to build an internal lib the same way that tests are built
# See $DEPOT/lib/dtv-htmlshellremote/CMakeList.txt for a use case
MACRO(Build_Internal_Lib name)
	set( src "${PROJECT_SOURCE_DIR}/${name}" )

	# Process project
	PROCESS_PROJECT_AUX( ${name} ${src} ${name}_info 1 )

	# Add components from project name
	LIST( APPEND ${name}_info_INTERNAL_COMPONENTS ${${PRE_BUILD_PROJECT_NAME}_info_INTERNAL_COMPONENTS} )
	LIST( APPEND ${name}_info_EXTERNAL_COMPONENTS ${${PRE_BUILD_PROJECT_NAME}_info_EXTERNAL_COMPONENTS} )

	# Build dependencies
	BUILD_PROJECT_DEPS( "lib" ${name} ${src} test_build )

	# Add library
	ADD_LIBRARY( ${name} SHARED ${${name}_info_SOURCE_FILES} )

	CLEAN_LIST( test_build_INCLUDE_DIRECTORIES )
	INCLUDE_DIRECTORIES( ${test_build_INCLUDE_DIRECTORIES} )

	# Link
	TARGET_LINK_LIBRARIES( ${name} ${test_build_LINK_LIBRARIES} )
ENDMACRO(Build_Internal_Lib)
