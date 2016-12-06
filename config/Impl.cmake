if( POLICY CMP0054 )
	cmake_policy(SET CMP0011 OLD)
	cmake_policy(SET CMP0054 OLD)
endif()

#Make the prebuild for each project of the lib/tool compilation.
MACRO (DoBuild type)
	message( STATUS "Begin setup build" )

	# Setup basic source options
	SETUP_BUILD( ${type} )

	# Process project
	PROCESS_PROJECT_AUX( ${PRE_BUILD_PROJECT_NAME} ${PROJECT_SOURCE_DIR} ${PRE_BUILD_PROJECT_NAME}_info 1 )

	# Define source groups (for VS)
	define_source_groups( ${${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES} )

	# Setup output
	SETUP_OUTPUT( ${type} out_dir )

	# Add project dependencies
	BUILD_PROJECT_DEPS( ${type} ${PRE_BUILD_PROJECT_NAME} ${src} build )

	# Add project definitions
	SETUP_DEFINITIONS()

	# Add project include directories
	LIST( APPEND build_INCLUDE_DIRECTORIES ${PROJECT_BINARY_DIR} )
	CLEAN_LIST( build_INCLUDE_DIRECTORIES )
	INCLUDE_DIRECTORIES( ${build_INCLUDE_DIRECTORIES} )

	# Add project link directories
	CLEAN_LIST( build_LINK_DIRECTORIES )
	link_directories( ${build_LINK_DIRECTORIES} )

	# Add project link libraries
	CLEAN_LIST( build_LINK_LIBRARIES )
	TARGET_LINK_LIBRARIES( ${PRE_BUILD_PROJECT_NAME} ${build_LINK_LIBRARIES} )

	# Show status
	SHOW_STATUS()

	# Write generated/config.h file
	Write_Config( ${type} )

	# Compile and run tests
	IF(COMPILE_TESTS AND PRE_BUILD_PROJECT_TESTS)
		DO_TESTS( "${PRE_BUILD_PROJECT_TESTS}" )
	ENDIF(COMPILE_TESTS AND PRE_BUILD_PROJECT_TESTS)

	# Setup doxygen
	PROCESS_DOXYGEN( ${PRE_BUILD_PROJECT_NAME} )

	# Install project
	installProject( ${PRE_BUILD_PROJECT_NAME} ${out_dir} )

	# Add additional targets
	add_extra_targets( ${PRE_BUILD_PROJECT_NAME} ${type} "" )

	message( STATUS "End setup build" )
ENDMACRO (DoBuild)

# Setup build
MACRO( SETUP_BUILD type )
	# To avoid problems when using the name in #define
	STRING(TOUPPER ${PRE_BUILD_PROJECT_NAME} PROJECT_NAME_UPPERCASE )
	STRING(REPLACE "-" "_" PROJECT_NAME_UPPERCASE_ESCAPED ${PROJECT_NAME_UPPERCASE} )
	LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

	# Get project source dir
	GET_SOURCE_DIR( ${type} ${PRE_BUILD_PROJECT_NAME} src )

	# Setup version
	SETUP_VERSION()

	IF(${type} STREQUAL "tool")
		setIfEmpty( PRE_BUILD_BIN_TYPE "bin" )
		# Load build configuration (see options target)
 		INCLUDE_IF_EXIST( ${PROJECT_BINARY_DIR}/../config.cmake )
		# Setup packaging
		SETUP_PACKAGING()
	ENDIF(${type} STREQUAL "tool")
ENDMACRO( SETUP_BUILD )

# Setup binary and returns the output directory
MACRO( SETUP_OUTPUT type out_dir )
	IF(${type} STREQUAL "lib")
		# Set output name and destination
		SET_LIBNAME( out_name ${PRE_BUILD_PROJECT_NAME} )
		set( out_dir "lib" )

		# Setup library build type
		IF(BUILD_SHARED)
			ADD_LIBRARY( ${PRE_BUILD_PROJECT_NAME} SHARED ${${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES} )
			stripBinary( ${PRE_BUILD_PROJECT_NAME} )
		ELSE(BUILD_SHARED)
			ADD_LIBRARY( ${PRE_BUILD_PROJECT_NAME} STATIC ${${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES} )
		ENDIF(BUILD_SHARED)
	ELSE()
		IF ("${PLATFORM_BINARY_TYPE}" STREQUAL "lib" OR "${PRE_BUILD_BIN_TYPE}" STREQUAL "lib")
			# Set output name and destination
			SET_LIBNAME( out_name ${PRE_BUILD_PROJECT_NAME} )
			set( out_dir "lib" )

			ADD_LIBRARY( ${PRE_BUILD_PROJECT_NAME} SHARED ${${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES} )
		ELSE()
		  # Setup executable
		  add_system_executable( ${PRE_BUILD_PROJECT_NAME} ${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES )

		  # Set output name and destination
		  SET_TOOLNAME( out_name ${PRE_BUILD_PROJECT_NAME} )
		  set( out_dir ${PRE_BUILD_BIN_DESTINATION} )
		ENDIF()

		# Add strip if necesary
		stripBinary( ${PRE_BUILD_PROJECT_NAME} )

		# Add package construction
		DO_PACKAGE()
	ENDIF(${type} STREQUAL "lib")
	set_target_properties( ${PRE_BUILD_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${out_name} )
ENDMACRO( SETUP_OUTPUT )

MACRO( SETUP_DEFINITIONS )
	LIST( APPEND build_DEFINITIONS "-DPROJECT_NAME=${PRE_BUILD_PROJECT_NAME}" )
	IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		LIST( APPEND build_DEFINITIONS "-D_DEBUG" )
	ELSE()
		LIST( APPEND build_DEFINITIONS "-DNDEBUG" )
		IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")	# TODO: Check this!
			SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
			SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
		ENDIF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	ENDIF()
	CLEAN_LIST( build_DEFINITIONS )
	add_definitions( ${build_DEFINITIONS} )
ENDMACRO( SETUP_DEFINITIONS )

# Setup version
MACRO( SETUP_VERSION )
	# Get revision from repository
	GET_GIT_REVISION( ${PRE_BUILD_PROJECT_NAME}_TAG_VERSION ${PRE_BUILD_PROJECT_NAME}_BUILD_VERSION ${PRE_BUILD_PROJECT_NAME}_HASH_VERSION )

	# Get from version.cmake if exists
	INCLUDE_IF_EXIST( ${src}/version.cmake )
	IF (${FILE_INCLUDED})
		SET(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${${PRE_BUILD_PROJECT_NAME}_BUILD_VERSION})
	ELSE()
		SET(PROJECT_VERSION 0.0.${${PRE_BUILD_PROJECT_NAME}_BUILD_VERSION})
	ENDIF()
ENDMACRO( SETUP_VERSION )

# Do a package target with cpack
MACRO( SETUP_PACKAGING )
	message( STATUS "Begin setup packaging" )
	IF (NOT "${PLATFORM}" STREQUAL "ANDROID")
		# Process global cpack variables
		INCLUDE_COMPONENTS( "${CONFIG_PATH}/package/cpack" )
	ENDIF ()
	message( STATUS "End setup packaging" )
ENDMACRO( SETUP_PACKAGING )

MACRO( DO_PACKAGE )
	message( STATUS "Begin do packaging" )
	IF ("${PLATFORM}" STREQUAL "ANDROID")
		INCLUDE_COMPONENTS( "${CONFIG_PATH}/package/Apk" )
		android_create_apk (${PRE_BUILD_PROJECT_NAME} "${CMAKE_BINARY_DIR}/apk" "${CMAKE_BINARY_DIR}/lib${PROJECT_PREFIX}${PRE_BUILD_PROJECT_NAME}.so" "share/${PRE_BUILD_PROJECT_NAME}")
	ELSE ("${PLATFORM}" STREQUAL "ANDROID")
		# Do the work!
		INCLUDE(CPack)
	ENDIF ("${PLATFORM}" STREQUAL "ANDROID")
	message( STATUS "End do packaging" )
ENDMACRO( DO_PACKAGE )

# Get all project
MACRO( BUILD_PROJECT_DEPS type name src out )
	IF (BUILD_SHARED)
		# Add direct internal dependencies (cflags and libs)
		get_internal_components( ${name}_info_INTERNAL_COMPONENTS internals )
		add_dependents_list( internals 1 1 ${out} )

		# Add direct external dependencies (cflags and libs)
		add_dependents_list( ${name}_info_EXTERNAL_COMPONENTS 1 1 ${out} )
	ELSE()
		# Add direct internal dependencies (only cflags)
		get_internal_components( ${name}_info_INTERNAL_COMPONENTS internals )
		add_dependents_list( internals 1 0 ${out} )

		# Add direct external dependencies (only cflags)
		add_dependents_list( ${name}_info_EXTERNAL_COMPONENTS 1 0 ${out} )

		# Add all internal dependencies if tool
		IF(${type} STREQUAL "tool")
			# Get all dependencies, in order
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
		ENDIF(${type} STREQUAL "tool")
	ENDIF (BUILD_SHARED)
ENDMACRO( BUILD_PROJECT_DEPS type name out )

MACRO(SetPlatformName)
	IF (NOT PLATFORM)
		IF (UNIX)
			set( PLATFORM UNIX )
		ELSE(MSVC EQUAL 1)
			set( PLATFORM VS )
		ENDIF (UNIX)
	ENDIF (NOT PLATFORM)
ENDMACRO(SetPlatformName)

# Returns project full name
macro(GET_PROJECT_FULL_NAME type name full)
	IF(EXISTS "${DEPOT}/${type}/${name}")
		SET( ${full} "${name}")
	ELSE(EXISTS "${DEPOT}/${type}/${name}")
		IF(EXISTS "${DEPOT}/${type}/${PROJECT_PREFIX}-${name}")
			SET( ${full} "${PROJECT_PREFIX}-${name}")
		ELSE()
			message(FATAL_ERROR "\n -- \t ERROR: Path for the project \"${name}\" not found.\n")
		ENDIF(EXISTS "${DEPOT}/${type}/${PROJECT_PREFIX}-${name}")
	ENDIF(EXISTS "${DEPOT}/${type}/${name}")
endmacro(GET_PROJECT_FULL_NAME)

# Returns project source dir
macro(GET_SOURCE_DIR type name src_dir )
	GET_PROJECT_FULL_NAME( ${type} ${name} proj_name )
	SET( ${src_dir} "${DEPOT}/${type}/${proj_name}")
endmacro(GET_SOURCE_DIR)

# Returns project directories: src and build
macro(GET_PROJECT_DIR type name src_dir build_dir)
	GET_PROJECT_FULL_NAME( ${type} ${name} proj_name )
	SET( ${src_dir} "${DEPOT}/${type}/${proj_name}")
	SET( ${build_dir} "${PROJECT_BINARY_DIR}/${proj_name}")
endmacro(GET_PROJECT_DIR)

# Pass internal component to component names
macro( get_internal_components names comps )
	FOREACH( name ${${names}} )
		STRING( TOUPPER ${name} comp )
		LIST( APPEND ${comps} "GINGALIB-${comp}" )
	ENDFOREACH( name ${${names}} )
endmacro( get_internal_components names comps )

# Parse each package and copy the cflags and libs necesary to build
macro(add_dependents_list comps cflags libs out)
	FOREACH( comp ${${comps}} )
		IF (${cflags} EQUAL 1)
			LIST( APPEND ${out}_DEFINITIONS         ${${comp}_DEFINITIONS} )
			LIST( APPEND ${out}_INCLUDE_DIRECTORIES ${${comp}_INCLUDE_DIRS} ${${comp}_INCLUDE_DIR} )
		ENDIF (${cflags} EQUAL 1)

		IF (${libs} EQUAL 1)
			LIST( APPEND ${out}_LINK_DIRECTORIES ${${comp}_LIBRARY_DIRS} )
			EXTRACT_LIBS( ${out}_LINK_LIBRARIES ${comp} )
		ENDIF (${libs} EQUAL 1)
	ENDFOREACH( comp ${comps} )
endmacro(add_dependents_list comps out)

# Strip binary to discards all symbols from the current compilation.
macro (stripBinary name)
	IF(${CMAKE_BUILD_TYPE} STREQUAL "Release" AND STRIP_TOOL)
		ADD_CUSTOM_COMMAND( TARGET ${name} POST_BUILD COMMAND ${STRIP_TOOL} $<TARGET_FILE:${name}> )
	ENDIF()
endmacro(stripBinary)

# Add all tests
MACRO(DO_TESTS tests)
	FOREACH( test ${tests} )
		# Set test name
		set( name "${PRE_BUILD_PROJECT_NAME}${test}" )
		set( src "${PROJECT_SOURCE_DIR}/${test}" )

		# Process project
		PROCESS_PROJECT_AUX( ${name} ${src} ${name}_info 1 )

		# Add components from project name
		LIST( APPEND ${name}_info_INTERNAL_COMPONENTS ${${PRE_BUILD_PROJECT_NAME}_info_INTERNAL_COMPONENTS} )
		LIST( APPEND ${name}_info_EXTERNAL_COMPONENTS ${${PRE_BUILD_PROJECT_NAME}_info_EXTERNAL_COMPONENTS} )

		# Build test dependencies
		BUILD_PROJECT_DEPS( "tool" ${name} ${src} test_build )

		# Add test exe
		add_system_executable( ${name} ${name}_info_SOURCE_FILES )

		# Link
		TARGET_LINK_LIBRARIES( ${name} ${PRE_BUILD_PROJECT_NAME} ${test_build_LINK_LIBRARIES} )

		# Write options
		SET( TEST_OUTPUT_CONFIG_FILE "${PROJECT_BINARY_DIR}/CMakeFiles/CMakeTmp/${name}_config.h.cin" )
		FILE( WRITE ${TEST_OUTPUT_CONFIG_FILE} "#pragma once\n\n")
		Write_Options( ${src} ${name} ${TEST_OUTPUT_CONFIG_FILE} "test.h" )

		IF(RUN_TESTS)
			ENABLE_TESTING()
			IF (EXISTS ${PROJECT_SOURCE_DIR}/test/test.sh)
				ADD_TEST( ${name} ${PROJECT_SOURCE_DIR}/test/test.sh ${CMAKE_BINARY_DIR}/${name} ${PROJECT_SOURCE_DIR})
			ELSE (EXISTS ${PROJECT_SOURCE_DIR}/test/test.sh)
				ADD_TEST( ${name} ${CMAKE_BINARY_DIR}/${name} )
			ENDIF(EXISTS ${PROJECT_SOURCE_DIR}/test/test.sh)
		ENDIF(RUN_TESTS)
	ENDFOREACH( test ${tests} )
ENDMACRO(DO_TESTS tests)

#Make Doxygen doc
MACRO(GET_DOXYGEN_BUILD_DIR var prefix)
	set( ${var} "${prefix}/doc/doxygen" )
ENDMACRO(GET_DOXYGEN_BUILD_DIR var prefix)

MACRO( CHECK_DOXYGEN generate)
	set( ${generate} 0 )
	IF (MAKE_DOXYGEN)
		FIND_PACKAGE(Doxygen)
		FIND_PACKAGE(LATEX)
		IF (NOT (DOXYGEN_FOUND AND LATEX_COMPILER AND PDFLATEX_COMPILER))
			IF (NOT DOXYGEN_FOUND)
				message(FATAL_ERROR "\n -- \t ERROR: Doxygen not found\n")
			ELSE()
				message(FATAL_ERROR "\n -- \t ERROR: LaTex not found\n")
			ENDIF()
		else()
			set( ${generate} 1 )
		ENDIF()
	ENDIF()
ENDMACRO( CHECK_DOXYGEN generate)

MACRO(DO_DOXYGEN name dst)
	CHECK_DOXYGEN( gen )
	IF (gen)
		# Get all dependencies, in order
		GET_SOURCE_DIR( "tool" ${PRE_BUILD_PROJECT_NAME} src )
		GET_PROJECT_BUILD_ORDER( "tool" ${PRE_BUILD_PROJECT_NAME} ${src} build_list 0 )
		LIST(REVERSE build_list)

		# Setup paths
		GET_DOXYGEN_BUILD_DIR( doxy_output ${CMAKE_CURRENT_BINARY_DIR} )
		set( DOXY_LATEX_REFMAN "${doxy_output}/out/latex/refman.tex" )
		set( DOXY_LATEX_FILE "${doxy_output}/dev.tex" )
		set( DOXY_LATEX_MANUAL "${doxy_output}/${name}.pdf" )

		# Generate dev.tex from dev.tex.in
		set( DOXY_PACKAGE "${doxy_output}/out/latex/doxygen" )
		set( COMMANDS_PACKAGE "${CONFIG_PATH}/doxygen/commands.tex" )
		CONFIGURE_FILE( "${CONFIG_PATH}/doxygen/dev.tex.in" ${DOXY_LATEX_FILE} @ONLY)

		# Import doxygen title
		FILE(APPEND ${DOXY_LATEX_FILE} "\\import*{${CONFIG_PATH}/doxygen/}{title.tex}\n" )
		FILE(APPEND ${DOXY_LATEX_FILE} "\\tableofcontents\n" )

		# Append tool documentation
		IF (EXISTS "${PROJECT_SOURCE_DIR}/doc/dev/${PRE_BUILD_PROJECT_NAME}.tex")
			FILE(APPEND ${DOXY_LATEX_FILE} "\\specialchapter\n" )
			FILE(APPEND ${DOXY_LATEX_FILE} "\\chapter{\\projectname}\n" )
			FILE(APPEND ${DOXY_LATEX_FILE} "\\import*{${PROJECT_SOURCE_DIR}/doc/dev/}{${PRE_BUILD_PROJECT_NAME}.tex}\n" )
		ENDIF (EXISTS "${PROJECT_SOURCE_DIR}/doc/dev/${PRE_BUILD_PROJECT_NAME}.tex")

		# Set regular chapter style
		FILE(APPEND ${DOXY_LATEX_FILE} "\\regularchapter\n" )

		# Add mainpages
		FILE(APPEND ${DOXY_LATEX_FILE} "\\part{Componentes}\n" )
		FOREACH(component ${build_list})
			GET_PROJECT_FULL_NAME( "lib" ${component} comp_full_name )
			GET_SOURCE_DIR( "lib" ${component} comp_src )

			IF (EXISTS "${comp_src}/doc/dev.tex")
				FILE(APPEND ${DOXY_LATEX_FILE} "\\chapter{Librería ${component}}\n" )
				FILE(APPEND ${DOXY_LATEX_FILE} "\\import*{${comp_src}/doc/}{dev.tex}\n" )
			ENDIF()
		ENDFOREACH(component ${build_list})

		# Add reference
		FILE(APPEND ${DOXY_LATEX_FILE} "\\part{Referencias}\n" )
		FILE(APPEND ${DOXY_LATEX_FILE} "\\chapter{${PRE_BUILD_PROJECT_NAME}}\n" )
		FILE(APPEND ${DOXY_LATEX_FILE} "\\import*{${doxy_output}/out/latex/}{refman.tex}\n" )
		FOREACH(component ${build_list})
			GET_PROJECT_FULL_NAME( "lib" ${component} comp_full_name )
			GET_DOXYGEN_BUILD_DIR( comp_doxy_out "${CMAKE_CURRENT_BINARY_DIR}/../${comp_full_name}" )
			FILE(APPEND ${DOXY_LATEX_FILE} "\\chapter{${component}}\n" )
			FILE(APPEND ${DOXY_LATEX_FILE} "\\import*{${comp_doxy_out}/out/latex/}{refman.tex}\n" )
		ENDFOREACH()
		FILE(APPEND ${DOXY_LATEX_FILE} "\\end{document}\n" )

		# Create latex from
		ADD_CUSTOM_COMMAND(
			OUTPUT ${DOXY_LATEX_MANUAL}
			DEPENDS ${DOXY_LATEX_FILE} ${DOXY_LATEX_REFMAN}
			COMMAND ${CONFIG_PATH}/pdflatex.py
			ARGS ${PDFLATEX_COMPILER} batchmode ${name} ${doxy_output} ${DOXY_LATEX_FILE}
			WORKING_DIRECTORY ${doxy_output}
			COMMENT "Create pdf from dev.tex"
		)

		# Add custom target doc to build documentation
		ADD_CUSTOM_TARGET( doc_${name} ALL echo
			DEPENDS ${DOXY_LATEX_MANUAL}
		)

		ADD_DEPENDENCIES( doc_${name} doxy_gen_${PRE_BUILD_PROJECT_NAME} )

		# Install manual
		INSTALL( FILES ${DOXY_LATEX_MANUAL}
			DESTINATION ${dst}
		)
	ENDIF()
ENDMACRO(DO_DOXYGEN name dst)

MACRO(PROCESS_DOXYGEN name)
	CHECK_DOXYGEN( gen )
	IF (gen)
		# Setup paths
		GET_DOXYGEN_BUILD_DIR( doxy_output ${CMAKE_CURRENT_BINARY_DIR} )
		set(DOXY_OUTPUT_DIR "${doxy_output}/out" )
		set(DOXY_OUTPUT_REFMAN "${DOXY_OUTPUT_DIR}/latex/refman.tex" )

		# Create Doxyfile config file
		setIfEmptyEnv(DOXY_GENERATE_HTML TVD_GENERATE_HTML "NO")
		set(DOXY_PROJECT_NAME "${PRE_BUILD_PROJECT_NAME}" CACHE STRING "Project name")
		set(DOXY_CONFIG_FILE "${doxy_output}/Doxyfile" )
		set(DOXY_LATEX_HEADER_FILE "${CONFIG_PATH}/doxygen/empty.tex" )
		set(DOXY_LATEX_FOOTER_FILE "${CONFIG_PATH}/doxygen/empty.tex" )
		IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/resources)
			set(DOXY_IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/doxygen/resources" CACHE PATH "Doxygen resources directory")
		ENDIF()
		set(DOXY_FILTER_FILES "" )
		FOREACH( f ${${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES} )
			set( DOXY_FILTER_FILES "${DOXY_FILTER_FILES} \\\n ${CMAKE_CURRENT_SOURCE_DIR}/${f}" )
		ENDFOREACH( f ${${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES} )
		set( LATEX_VERSION ${PROJECT_VERSION} )

		CONFIGURE_FILE(${CONFIG_PATH}/doxygen/Doxyfile.in ${DOXY_CONFIG_FILE} @ONLY)

		# Create doxygen output directory
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${DOXY_OUTPUT_DIR}
			COMMAND   cmake -E make_directory ${DOXY_OUTPUT_DIR}
			COMMENT   "Create doxygen output directory"
		)

		# Run doxygen on config file and generate refman.tex
		ADD_CUSTOM_COMMAND(
			OUTPUT ${DOXY_OUTPUT_REFMAN}
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG_FILE}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Generating reference manual"
			DEPENDS ${DOXY_OUTPUT_DIR}
		)

		ADD_CUSTOM_TARGET( doxy_gen_${name} ALL echo
			DEPENDS ${DOXY_OUTPUT_REFMAN}
		)
	ENDIF()
ENDMACRO(PROCESS_DOXYGEN)

# Make all docs
MACRO(MAKE_DOC name tex src dst)
	# Make documentation
	IF (MAKE_DOCUMENTATION)
		FIND_PACKAGE(LATEX)
		IF (LATEX_COMPILER AND PDFLATEX_COMPILER)
			set( output ${CMAKE_CURRENT_BINARY_DIR}/doc/${name} )
			set( srcDir ${CMAKE_CURRENT_SOURCE_DIR}/${src} )
			set( texFile ${CMAKE_CURRENT_BINARY_DIR}/doc/${tex} )
			set( LATEX_VERSION ${PROJECT_VERSION} )
			CONFIGURE_FILE( ${srcDir}/${tex} ${texFile} @ONLY)

			#	Make output directory
			ADD_CUSTOM_COMMAND(
				OUTPUT    ${output}
				COMMAND   cmake -E make_directory ${output}
				COMMENT   "Create LaTeX output directory: ${output}"
			)

			#	Compile tex file to pdf
			ADD_CUSTOM_COMMAND(
				OUTPUT    ${output}/${name}.pdf
				DEPENDS   ${output} ${texFile}
				COMMAND   ${CONFIG_PATH}/pdflatex.py
				ARGS      ${PDFLATEX_COMPILER} batchmode "${name}" "${output}" "${texFile}"
				WORKING_DIRECTORY ${srcDir}
				COMMENT   "pdflatex"
			)

			#	Add custom target doc to build documentation
			ADD_CUSTOM_TARGET( doc_${name} ALL echo
				DEPENDS ${output}/${name}.pdf
			)

			#	Install pdf
			INSTALL( FILES ${output}/${name}.pdf
				DESTINATION ${dst}
			)
		ELSE()
			message(FATAL_ERROR "\n -- \t ERROR: LaTeX not found\n")
		ENDIF()
	ENDIF (MAKE_DOCUMENTATION)
ENDMACRO(MAKE_DOC)

# Install the project into its corresponding directory
MACRO(installProject PROJECT_NAME DESTINATION)
	INSTALL( TARGETS ${PRE_BUILD_PROJECT_NAME}
		 DESTINATION ${DESTINATION}
		 CONFIGURATIONS ${CMAKE_BUILD_TYPE}
		 PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
			     GROUP_READ 	    GROUP_EXECUTE
			     WORLD_READ 	    WORLD_EXECUTE )
ENDMACRO(installProject)

# Show log of the compiled project.
macro(SHOW_STATUS)
	# display status message for important variables
	MESSAGE( STATUS )
	MESSAGE( STATUS "-------------------------------------------------------------------------------" )
	MESSAGE( STATUS "PROJECT_NAME = ${PRE_BUILD_PROJECT_NAME}" )
	MESSAGE( STATUS "PROJECT_DESCRIPTION = ${PRE_BUILD_PROJECT_DESCRIPTION}" )
	MESSAGE( STATUS "PROJECT_VENDOR = ${PROJECT_VENDOR}" )
	MESSAGE( STATUS "PROJECT_VERSION = ${PROJECT_VERSION}" )
	MESSAGE( STATUS "PLATFORM = ${PLATFORM}" )
	MESSAGE( STATUS "SYSTEM = ${CMAKE_SYSTEM_NAME}" )
	MESSAGE( STATUS "USER CONFIG FILE: ${SETUP_USER}" )
	MESSAGE( STATUS "PLATFORM_FILE = ${CMAKE_TOOLCHAIN_FILE}" )
	MESSAGE( STATUS "BUILD_SHARED = ${BUILD_SHARED}" )
	MESSAGE( STATUS "BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
	MESSAGE( STATUS "SILENT = ${SILENT}" )
	MESSAGE( STATUS "COMPILE_TESTS = ${COMPILE_TESTS}" )
	MESSAGE( STATUS "RUN_TESTS = ${RUN_TESTS}" )
	MESSAGE( STATUS "MAKE_DOCUMENTATION = ${MAKE_DOCUMENTATION}" )
	MESSAGE( STATUS "MAKE_DOXYGEN = ${MAKE_DOXYGEN}" )
	MESSAGE( STATUS "INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
	MESSAGE( STATUS "CONFIG_INSTALL_PREFIX = ${CMAKE_CONFIG_INSTALL_PREFIX}" )
	MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
	MESSAGE( STATUS "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}" )
	MESSAGE( STATUS "CMAKE_SYSTEM_PREFIX_PATH = ${CMAKE_SYSTEM_PREFIX_PATH}" )
	MESSAGE( STATUS "CMAKE_FIND_ROOT_PATH = ${CMAKE_FIND_ROOT_PATH}" )
	MESSAGE( STATUS "INTERNAL DEPENDENCIES = ${${PRE_BUILD_PROJECT_NAME}_info_INTERNAL_COMPONENTS}")
	MESSAGE( STATUS "EXTERNAL DEPENDENCIES = ${${PRE_BUILD_PROJECT_NAME}_info_EXTERNAL_COMPONENTS}")
	Show_Options()
	if(${VERBOSITY} GREATER 0)
		MESSAGE( STATUS "DEFINITIONS = ${build_DEFINITIONS}" )
		MESSAGE( STATUS "INCLUDE_DIRECTORIES = ${build_INCLUDE_DIRECTORIES}" )
		MESSAGE( STATUS "LINK_DIRECTORIES = ${build_LINK_DIRECTORIES}" )
		MESSAGE( STATUS "LINK_LIBRARIES = ${build_LINK_LIBRARIES}")
	endif(${VERBOSITY} GREATER 0)
	IF(${VERBOSITY} GREATER 1)
		MESSAGE( STATUS "-------------------------------------------------------------------------------" )
		MESSAGE( STATUS "HEADER FILES = ${${PRE_BUILD_PROJECT_NAME}_info_HEADER_FILES}" )
		MESSAGE( STATUS "SOURCE FILES = ${${PRE_BUILD_PROJECT_NAME}_info_SOURCE_FILES}" )
	ENDIF(${VERBOSITY} GREATER 1)
	MESSAGE( STATUS "-------------------------------------------------------------------------------" )
endmacro(SHOW_STATUS)

MACRO (Write_Custom_Options dir src dst )
	if(EXISTS ${dir}/${src})
		FILE( APPEND ${dst} "\n//\tOptions from ${src}\n")
		FILE( READ ${dir}/${src} CONFIG_PROJECT )
		FILE( APPEND ${dst} "${CONFIG_PROJECT}" )
	endif(EXISTS ${dir}/${src})
ENDMACRO (Write_Custom_Options)

MACRO( Show_Options)
	MESSAGE( STATUS "OPTIONS = ${${PRE_BUILD_PROJECT_NAME}_info_OPTIONS_NAMES}")
	FOREACH(group ${${PRE_BUILD_PROJECT_NAME}_info_OPTIONS_NAMES})
		set( tmp "" )
		FOREACH(opt ${${group}_OPTIONS})
			IF(${group}_USE_${opt})
				LIST( APPEND tmp "${opt}" )
			ENDIF(${group}_USE_${opt})
		ENDFOREACH(opt ${${group}_OPTIONS})
		if (NOT "${tmp}" STREQUAL "")
			MESSAGE( STATUS "\t${group}(${tmp})" )
		endif()
	ENDFOREACH()
ENdMACRO(Show_Options)

MACRO( Write_Options src proj out cfg)
	# Append autogenerated options
	FILE(APPEND ${out} "\n")
	FOREACH(group ${${proj}_info_OPTIONS_NAMES})
		FILE(APPEND ${out} "//\t${group} OPTIONS\n")
		#define all groups options with their values
		FOREACH(opt ${${group}_OPTIONS})
			FILE(APPEND ${out} "#define ${group}_USE_${opt} ${${group}_USE_${opt}}\n")
			IF(${group}_USE_${opt})
				STRING(TOUPPER ${opt} OPT_UPPER)
				WRITE_CUSTOM_OPTIONS( ${src} "config.${group}_${OPT_UPPER}.h.cin" ${out} )
			ENDIF(${group}_USE_${opt})
		ENDFOREACH(opt ${${group}_OPTIONS})
		FILE(APPEND ${out} "\n")
	ENDFOREACH(group ${${proj}_info_OPTIONS_NAMES})

	# Write custom project config data
	WRITE_CUSTOM_OPTIONS( ${src} "config.h.cin" ${out} )

	CONFIGURE_FILE( ${out} ${PROJECT_BINARY_DIR}/generated/${cfg} )
ENDMACRO( Write_Options proj out )

# Save all configs from the current lib/tool compilation into a config.h for the compiled project.
MACRO (Write_Config buildType)
	SET( OUTPUT_CONFIG_FILE "${PROJECT_BINARY_DIR}/CMakeFiles/CMakeTmp/${PRE_BUILD_PROJECT_NAME}_config.h.cin" )

	# Define macros foreach values are appended into tmp config of current project.
	FILE(WRITE ${OUTPUT_CONFIG_FILE} "#pragma once\n\n")
	FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_NAME " "\"${PRE_BUILD_PROJECT_NAME}\"\n")
	FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_DESCRIPTION " "\"${PRE_BUILD_PROJECT_DESCRIPTION}\"\n")
	FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_VENDOR " "\"${PROJECT_VENDOR}\"\n")
	FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_VERSION " "\"${PROJECT_VERSION}\"\n")
	FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_REPO_VERSION " "\"${${PRE_BUILD_PROJECT_NAME}_HASH_VERSION}\"\n\n")
	if(${buildType} STREQUAL "tool")
		if("${PRE_BUILD_PROJECT_SERVICE}" STREQUAL "ON")
			FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_IS_SERVICE true\n")
		else()
			FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_IS_SERVICE false\n")
		endif()
		FILE(APPEND ${OUTPUT_CONFIG_FILE} "#define ${PROJECT_NAME_UPPERCASE_ESCAPED}_CONFIG " "\"${CMAKE_BUILD_CONFIG}\"\n")
	endif(${buildType} STREQUAL "tool")

	Write_Options( ${PROJECT_SOURCE_DIR} ${PRE_BUILD_PROJECT_NAME} ${OUTPUT_CONFIG_FILE} "config.h")
ENDMACRO (Write_Config)

# Check all project dependencies and returns the ordered build list of internal components
MACRO(GET_PROJECT_BUILD_ORDER type project src build_list only_ints)
	message( STATUS "Begin build list order" )

	set( all_comps "" )

	# Set variable only if we are getting internals dependencies
	if (${only_ints})
		set( GET_ONLY_INTERNALS 1 )
	endif (${only_ints})

	# Process main project information
	PROCESS_PROJECT_AUX( ${project} ${src} ${project}_info 0 )
	SET( comps ${${project}_info_INTERNAL_COMPONENTS} )
	CLEAN_LIST( comps )

	# Get all components that project depend
	WHILE (comps)
		# Pop first item
		LIST( GET comps 0 comp )
		LIST( REMOVE_AT comps 0 )

		# Add component to visited list
		LIST( APPEND all_comps ${comp} )

		# Process component
		PROCESS_PROJECT( "lib" ${comp} ${comp}_info 0 )

		# For each component in internal components
		FOREACH( int ${${comp}_info_INTERNAL_COMPONENTS})
			# Add int component if not in visited list
			LIST_CONTAINS( contains ${int} ${all_comps} )
			IF (NOT contains)
				LIST( APPEND comps ${int} )
			ENDIF (NOT contains)
		ENDFOREACH( int ${${comp}_info_INTERNAL_COMPONENTS})

		LIST(REMOVE_DUPLICATES comps)
	ENDWHILE (comps)

	# Reset internal dependency variable
	if (${only_ints})
		unset( GET_ONLY_INTERNALS )
	endif (${only_ints})

	# Remove duplicates, and the result is all internal components to build
	LIST(REMOVE_DUPLICATES all_comps)

	# Order the build list
	set( build_comps "" )
	set( build_complete 0 )
	WHILE (${build_complete} EQUAL 0)
		set( builds 0 )

		# Try build at least one component
		FOREACH( comp ${all_comps})
			# IF all comp dependecies are build, add to list the build
			set( need_build 1 )
			FOREACH( int ${${comp}_info_INTERNAL_COMPONENTS} )
				LIST(FIND build_comps ${int} pos)
				IF (${pos} EQUAL -1)
					set( need_build 0 )
				ENDIF (${pos} EQUAL -1)
			ENDFOREACH( int ${${comp}_info_INTERNAL_COMPONENTS} )

			IF (${need_build} EQUAL 1)
				LIST( APPEND build_comps ${comp} )
				LIST( REMOVE_ITEM all_comps ${comp} )
				SET( builds 1 )
			ENDIF (${need_build} EQUAL 1)
		ENDFOREACH( comp ${all_comps})

		# Check if we can finish
		IF( ${builds} EQUAL 0)
			set( build_complete 1 )
		ENDIF (${builds} EQUAL 0)
	ENDWHILE (${build_complete} EQUAL 0)

	SET( ${build_list} ${build_comps} )
	message(STATUS "Build list: ${build_comps}")
ENDMACRO(GET_PROJECT_BUILD_ORDER)

# Define groups from the source code files
macro(define_source_groups)
	set(CURRENT_GROUP "")
	set(CURRENT_GROUP_FILES "")
	foreach(file ${ARGN})
		#Get, by regex, the substring between the first "/" and the last "/" to define a source group
		#Ex: src/impl/.../aFile.h, with regex, obtains "/impl/.../"
		STRING(REGEX MATCH "[/\\](.*)[/\\]" SOURCE_GROUP_NAME ${file})
		string(LENGTH "${SOURCE_GROUP_NAME}" length )

		if(${length} GREATER 0)
			#If SOURCE_GROUP_NAME is a valid string, that needs remove the first "/" and the last "/" to define a valid source group.
			#Ex: regex obtains "/impl/.../", now remove the first "/" and the last "/" => "impl/..."
			MATH(EXPR length "${length}-2" )
			string(SUBSTRING ${SOURCE_GROUP_NAME} 1 ${length} SOURCE_GROUP_NAME)
			IF ( WIN32 )
				STRING( REGEX REPLACE "/" "\\\\" SOURCE_GROUP_NAME ${SOURCE_GROUP_NAME} )
			ENDIF ( WIN32 )
		endif(${length} GREATER 0)

		if("${SOURCE_GROUP_NAME}" EQUAL "${CURRENT_GROUP}")
			LIST(APPEND CURRENT_GROUP_FILES ${file})
		else("${SOURCE_GROUP_NAME}" EQUAL "${CURRENT_GROUP}")
			source_group("${CURRENT_GROUP}" FILES ${CURRENT_GROUP_FILES})
			set(CURRENT_GROUP ${SOURCE_GROUP_NAME})
			set(CURRENT_GROUP_FILES "")
			LIST(APPEND CURRENT_GROUP_FILES ${file})
		endif("${SOURCE_GROUP_NAME}" EQUAL "${CURRENT_GROUP}")
	endforeach(file ${PRE_BUILD_PROJECT_NAME}_SOURCE_FILES)
endmacro(define_source_groups)

# Process a project, get all information and puts result into proj_data
MACRO( PROCESS_PROJECT type project_name proj_data allComps )
	GET_SOURCE_DIR( ${type} ${project_name} dir )
	PROCESS_PROJECT_AUX( ${project_name} ${dir} ${proj_data} ${allComps} )
ENDMACRO( PROCESS_PROJECT )

# Process a project, get information and puts result into project_vars
# If allComps is
#     0 -> deps
#     1 -> deps,sources,install
MACRO( PROCESS_PROJECT_AUX project_name project_dir project_vars allComps )
	message( STATUS "Begin parse project: ${project_name} ${project_dir}")

	# Initialize project variables
	INIT_PROCESS_VARIABLES()

	# Process project files
	INCLUDE_PROJECT_COMPONENTS( ${project_dir} "deps" )
	IF(${allComps} EQUAL 1)
		INCLUDE_PROJECT_COMPONENTS( ${project_dir} "sources" )
		INCLUDE_PROJECT_COMPONENTS( ${project_dir} "install" )
		INCLUDE_PROJECT_COMPONENTS( ${project_dir} "cpack" )
	ENDIF()

	# Copy project variables
	COPY_PROCESS_VARIABLES( ${project_vars} )

	# Add header to sources
	LIST( APPEND ${project_vars}_SOURCE_FILES ${${project_vars}_HEADER_FILES} )

	message( STATUS "End parse project: ${project_name}")
ENDMACRO( PROCESS_PROJECT_AUX )

# Cleanup project variables. Used by PROCESS_PROJECT macro
MACRO( INIT_PROCESS_VARIABLES )
	SET( OPTIONS_NAMES "")
	SET( HEADER_FILES "")
	SET( SOURCE_FILES "")
	SET( INTERNAL_COMPONENTS "")
	SET( EXTERNAL_COMPONENTS "")
ENDMACRO( INIT_PROCESS_VARIABLES )

# Copy and unset project variables. Used by PROCESS_PROJECT macro
MACRO( COPY_PROCESS_VARIABLES var )
	COPY_AND_CLEAN_VARIABLE( ${var} OPTIONS_NAMES )
	COPY_AND_CLEAN_VARIABLE( ${var} HEADER_FILES )
	COPY_AND_CLEAN_VARIABLE( ${var} SOURCE_FILES )
	COPY_AND_CLEAN_VARIABLE( ${var} INTERNAL_COMPONENTS )
	COPY_AND_CLEAN_VARIABLE( ${var} EXTERNAL_COMPONENTS )
ENDMACRO( COPY_PROCESS_VARIABLES )

# Include components
MACRO(INCLUDE_COMPONENTS prefix)
	INCLUDE_IF_EXIST( ${prefix}.cmake )
	INCLUDE_IF_EXIST( ${prefix}.PLATFORM_${PLATFORM}.cmake )
	INCLUDE_IF_EXIST( ${prefix}.SYSTEM_${CMAKE_SYSTEM_NAME}.cmake )
ENDMACRO(INCLUDE_COMPONENTS prefix)

# Include components (deps/sources) and optional files of the current project compilation.
MACRO(INCLUDE_PROJECT_COMPONENTS path includePrefix)
	INCLUDE_COMPONENTS( "${path}/${includePrefix}" )

	# Process options
	FOREACH(group ${OPTIONS_NAMES})
		FOREACH(opt ${${group}_OPTIONS})
			IF(${group}_USE_${opt})
				STRING(TOUPPER ${opt} OPT_UPPER)
				SET(CURRENT_GROUP_OPTION ${group}_USE_${opt})
				INCLUDE_IF_EXIST(${path}/${includePrefix}.${group}_${OPT_UPPER}.cmake)
				UNSET(CURRENT_GROUP_OPTION)
			ENDIF()
		ENDFOREACH(opt)
	ENDFOREACH(group)
ENDMACRO(INCLUDE_PROJECT_COMPONENTS path includePrefix)

# Get the library file referenced by each member of the list
MACRO(EXTRACT_LIBS outputvar component)
	set(tmp_list_DEBUG "")
	Set(tmp_list_RELEASE "")
	Set(tmp_list_GENERAL "")

	SET(_list_suffix _GENERAL)
	FOREACH(lib ${${component}_LIBRARIES})
		IF("${lib}" STREQUAL "optimized")
				SET(_list_suffix _RELEASE)
		ELSE("${lib}" STREQUAL "optimized")
			IF("${lib}" STREQUAL "debug")
				SET(_list_suffix _DEBUG)
			ELSE("${lib}" STREQUAL "debug")
				IF("${lib}" STREQUAL "general")
					SET(_list_suffix _GENERAL)
				ELSE("${lib}" STREQUAL "general")
					LIST(APPEND tmp_list${_list_suffix} ${lib})
				ENDIF("${lib}" STREQUAL "general")
			ENDIF("${lib}" STREQUAL "debug")
		ENDIF("${lib}" STREQUAL "optimized")
	ENDFOREACH(lib ${${component}_LIBRARIES}})

	if(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel" )
		LIST(APPEND ${outputvar} ${tmp_list_GENERAL} ${tmp_list_RELEASE} )
	else()
		LIST(APPEND ${outputvar} ${tmp_list_GENERAL} ${tmp_list_DEBUG} )
	endif(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel" )

	# HACK remove first occurrences on REMOVE_DUPLICATES
	LIST( REVERSE ${outputvar} )
	LIST( REMOVE_DUPLICATES ${outputvar} )
	LIST( REVERSE ${outputvar} )
ENDMACRO(EXTRACT_LIBS outputvar component)

# Add packages from the Boost libraries, returning their names in "where"
# This macro is used to avoid calling find_package for packages that are already added
MACRO(ADD_BOOST_COMPONENTS_IMPL prefix )
	SET(components "")
	SET(components_length 0)

	FOREACH(component ${ARGN})
		STRING(TOUPPER ${component} COMP)
		STRING(COMPARE EQUAL "${Boost_${COMP}_LIBRARIES}" "" not_added)
		IF (not_added)
			LIST(APPEND components ${component})
			MATH(EXPR components_length "${components_length}+1")
		ENDIF()
	ENDFOREACH()

	IF(components_length)
		find_package( Boost 1.46.0 REQUIRED ${components})
		ADD_DEPENDENTS_IMPL( Boost )

		FOREACH(component ${components})
			STRING(TOUPPER ${component} COMP)
			ADD_DEPENDENTS_IMPL( Boost_${COMP} )
		ENDFOREACH(component ${components})
	ENDIF()

	UNSET(components)
ENDMACRO(ADD_BOOST_COMPONENTS_IMPL)

MACRO(ADD_INTERNAL_COMPONENTS_IMPL prefix)
	IF(GET_ONLY_INTERNALS)
		LIST(APPEND ${prefix}INTERNAL_COMPONENTS ${ARGN})
	ELSE()
		find_package( GingaLib 0.11.0 REQUIRED ${ARGN} )
		FOREACH(component ${GINGALIB_NAMES_COMPONENTS})
			STRING(TOUPPER ${component} COMP)
			CHECK_DEP_FOUND( "GINGALIB-${COMP}" )
		ENDFOREACH(component ${GINGALIB_NAMES_COMPONENTS})
		LIST(APPEND ${prefix}INTERNAL_COMPONENTS ${GINGALIB_NAMES_COMPONENTS})
	ENDIF()
ENDMACRO(ADD_INTERNAL_COMPONENTS_IMPL)

macro(CHECK_DEP_FOUND)
	foreach(dep ${ARGN})
		IF (NOT ${dep}_FOUND)
			message(FATAL_ERROR "NOT FOUNDED PACKAGE: ${dep}")
		endif()
	endforeach()
endmacro(CHECK_DEP_FOUND)

# Add external dependencies to the current project processed
macro(ADD_DEPENDENTS_IMPL)
	CHECK_DEP_FOUND( ${ARGN} )
	LIST(APPEND EXTERNAL_COMPONENTS ${ARGN})
	if(${VERBOSITY} GREATER 0)
		message(STATUS "Appended [${ARGN}] into EXTERNAL_COMPONENTS")
	endif(${VERBOSITY} GREATER 0)
endmacro(ADD_DEPENDENTS_IMPL)

# Add cmake command
macro( add_cmake_build_cmd out_dir tgt opts result)
	set( ${result} cmake --build . --config ${CMAKE_BUILD_TYPE} )
	if (NOT "${tgt}" STREQUAL "")
		set( ${result} ${${result}} --target ${tgt} )
	endif (NOT "${tgt}" STREQUAL "")

	if (NOT "${opts}" STREQUAL "")
		set( ${result} ${${result}} -- "${opts}" )
	endif (NOT "${opts}" STREQUAL "")
endmacro( add_cmake_build_cmd )

# Options variable
macro( GENERATE_OPTIONS name components )
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n" )
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}# SetupUser.cmake options" )
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n\\\\n" )

	SET(GLOBAL_VARS "CMAKE_INSTALL_PREFIX;CMAKE_CONFIG_INSTALL_PREFIX;CMAKE_PREFIX_PATH;CMAKE_BUILD_TYPE;PLATFORM_BINARY_TYPE;COMPILE_TESTS;RUN_TESTS;MAKE_DOCUMENTATION;MAKE_DOXYGEN;SILENT;PROJECT_PREFIX;PROJECT_RELEASE_CANDIDATE;PROJECT_VENDOR")

	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}# Global variables" )
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n\\\\n" )
	FOREACH(GLOBAL_VAR ${GLOBAL_VARS})
		set( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}SET\(${GLOBAL_VAR} ${${GLOBAL_VAR}})\\\\n")
	ENDFOREACH(GLOBAL_VAR ${GLOBAL_VARS})
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n\\\\n" )

	SET(GLOBAL_OPTIONS "BUILD_SHARED")
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}# Global options" )
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n\\\\n" )
	FOREACH(GLOBAL_OPTION ${GLOBAL_OPTIONS})
		set( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}OPTION\(${GLOBAL_OPTION} ${GLOBAL_OPTION} ${${GLOBAL_OPTION}})\\\\n")
	ENDFOREACH(GLOBAL_OPTION ${GLOBAL_OPTIONS})
	SET( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n\\\\n" )

	ADD_OPTIONS_TO_VARIABLE( ${name} )
	FOREACH(component ${components})
		ADD_OPTIONS_TO_VARIABLE( ${component} )
	ENDFOREACH(component)

	# Write CMAKE_BUILD_CONFIG to config.cmake
	FILE(WRITE "${PROJECT_BINARY_DIR}/config.cmake" "set ( CMAKE_BUILD_CONFIG \"${CMAKE_BUILD_CONFIG}\" )\n")

endmacro( GENERATE_OPTIONS )

macro( ADD_OPTIONS_TO_VARIABLE name )
	STRING(TOUPPER ${name} name_uppercase )
	set( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}# ${name_uppercase} options" )
	set( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n\\\\n" )
	FOREACH(group ${${name}_info_OPTIONS_NAMES})
		set( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}# ${group}\\\\n" )
		FOREACH(opt ${${group}_OPTIONS})
			set( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}SET\(${group}_USE_${opt} ${${group}_USE_${opt}})\\\\n" )
		ENDFOREACH(opt)
		set( CMAKE_BUILD_CONFIG "${CMAKE_BUILD_CONFIG}\\\\n" )
	ENDFOREACH(group)
endmacro( ADD_OPTIONS_TO_VARIABLE )

# Add a new target
macro( add_target name type is_default dependencies )
	# Get directories
	GET_PROJECT_DIR( ${type} ${name} src_dir build_dir )

	# Create directory
	execute_process( COMMAND cmake -E make_directory ${build_dir} )

	# Setup build cmd (from platform/system)
	SET_BUILD_TARGET( ${build_dir} build_cmd install_cmd )

	if(${is_default})
		SET(_all ALL)
	endif()

	# obtain the debug param passed to this cmake.
	PARSE_VERBOSITY( ${VERBOSITY} verbose_param )

	# set cmake cmd
	set( cmake_cmd cmake ${verbose_param} "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" -DCMAKE_PROJECT_NAME=${name} )
	IF (CMAKE_TOOLCHAIN_FILE)
		set( cmake_cmd ${cmake_cmd} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} )
	ENDIF (CMAKE_TOOLCHAIN_FILE)
	IF (${VERBOSITY} GREATER 0)
		set( cmake_cmd ${cmake_cmd} -DVERBOSITY=${VERBOSITY} -DCMAKE_VERBOSE_MAKEFILE=ON )
	ENDIF (${VERBOSITY} GREATER 0)
	set( cmake_cmd ${cmake_cmd} ${src_dir} )

	# Add target to build
	ADD_CUSTOM_TARGET(${name} ${_all}
		COMMAND cmake -E echo ${cmake_cmd}
		COMMAND ${cmake_cmd}
		COMMAND cmake -E echo ${build_cmd}
		COMMAND ${build_cmd}
		COMMAND cmake -E echo "test! ..."
		COMMAND ctest -V  # run tests
		COMMAND cmake -E echo ${install_cmd}
		COMMAND ${install_cmd}
		COMMAND cmake -E echo ${pack_cmd}
		COMMAND ${pack_cmd}                             # and pack if requested
		WORKING_DIRECTORY "${build_dir}"
		VERBATIM
	)

	# Add pack to default target
	if(${is_default})
		add_cmake_build_cmd( ${build_dir} package "" pack2_cmd)
		ADD_CUSTOM_TARGET(
			"pack"
			COMMAND ${pack2_cmd}
			WORKING_DIRECTORY "${build_dir}"
		)
	endif()

	# Add dependencies
	FOREACH(dep ${dependencies})
		ADD_DEPENDENCIES( ${name} ${dep} )
	ENDFOREACH(dep ${dependencies})
endmacro( add_target )

# Extra targets
macro( ADD_EXTRA_TARGETS name type components )
	ADD_EXTRA_TARGET( ${name} ${type} eclipse -G "Eclipse CDT4 - Unix Makefiles" "${components}" )
	ADD_EXTRA_TARGET( ${name} ${type} KDevelop3 -G "KDevelop3" "${components}" )
	ADD_EXTRA_TARGET( ${name} ${type} SublimeText2 -G "Sublime Text 2 - Unix Makefiles" "${components}" )
	ADD_EXTRA_TARGET( ${name} ${type} Xcode -G "Xcode" "${components}" )
	ADD_DEPS_TARGET( ${name} "${components}" )
endmacro( ADD_EXTRA_TARGETS )

# Aux targets functions
macro( ADD_EXTRA_TARGET name type target option param components )
	ADD_CUSTOM_TARGET( "${target}" )
	ADD_CMD_TO_TARGET( ${name} ${type} ${target} ${option} ${param}  )
	FOREACH(component ${components})
		ADD_CMD_TO_TARGET( ${component} "lib" ${target} ${option} ${param}  )
	ENDFOREACH(component)
endmacro( ADD_EXTRA_TARGET )

macro( ADD_CMD_TO_TARGET name type target option param )
	GET_PROJECT_DIR( ${type} ${name} src_dir build_dir )
	IF(NOT EXISTS "${build_dir}")
		SET( build_dir ${PROJECT_BINARY_DIR} )
	ENDIF()
	ADD_CUSTOM_COMMAND(
		TARGET "${target}"
		PRE_BUILD
		COMMAND cmake -E chdir ${build_dir} cmake ${option} ${param} -DCMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN_FILE}" ${src_dir} )
endmacro( ADD_CMD_TO_TARGET )

macro( ADD_ECHO_TO_TARGET target msg )
	ADD_CUSTOM_COMMAND( TARGET "${target}" PRE_BUILD COMMAND cmake -E echo ${msg} )
endmacro( ADD_ECHO_TO_TARGET )

# Options target
macro( ADD_OPTIONS_TARGET name components )
	set( target "options" )
	ADD_CUSTOM_TARGET( "${target}" )

	string(REPLACE "\\\\n" " ;" CMAKE_BUILD_CONFIG_LIST "${CMAKE_BUILD_CONFIG}")
	foreach(line ${CMAKE_BUILD_CONFIG_LIST})
		string(REPLACE "#" "'\#'" line ${line})
		string(REPLACE "SET(" "'SET('" line ${line})
		string(REPLACE "OPTION(" "'OPTION('" line ${line})
		string(REPLACE ")" "')'" line ${line})
		ADD_ECHO_TO_TARGET( ${target} "${line}")
	endforeach(line ${CMAKE_BUILD_CONFIG_LIST})
endmacro( ADD_OPTIONS_TARGET )

# Files target
macro( ADD_FILES_TARGET name type components )
	SET( target "files" )
	ADD_CUSTOM_TARGET( "${target}" )
	ADD_ECHO_TO_TARGET( ${target} "\n" )

	SET(name ${name})

	ENV2LIST("GENERIC_PROJECT_FILENAMES" generic_filenames )

	FOREACH(filename ${generic_filenames})
		IF(EXISTS "${DEPOT}/${filename}")
			ADD_ECHO_TO_TARGET( ${target} "deploy: ${filename}" )
		ENDIF(EXISTS "${DEPOT}/${filename}")
	ENDFOREACH(filename ${generic_filenames})

	ADD_FILES_TO_TARGET( ${name} ${target} ${type} )
	FOREACH(component ${components})
		ADD_FILES_TO_TARGET( ${component} ${target} "lib" )
	ENDFOREACH(component)
	ADD_ECHO_TO_TARGET( ${target} "\n" )
endmacro( ADD_FILES_TARGET )

macro( ADD_FILES_TO_TARGET name target  type )
	SET(name ${name})

	ENV2LIST("GENERIC_PRODUCT_FILENAMES" generic_filenames )
	ENV2LIST("CUSTOM_FILENAMES" custom_filenames )

	GET_PROJECT_FULL_NAME( ${type}  ${name} full )

	SET( GET_ONLY_INTERNALS 1 )

	# save values
	set( PROJECT_SOURCE_DIR_BAK ${PROJECT_SOURCE_DIR} )
	set( PRE_BUILD_PROJECT_NAME_BAK ${PRE_BUILD_PROJECT_NAME} )

	set( PROJECT_SOURCE_DIR "${DEPOT}/${type}/${full}/" )
	set( PRE_BUILD_PROJECT_NAME ${full} )
	PROCESS_PROJECT( ${type} ${name} ${name}_info 1 )

	# restore values
	set( PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_BAK} )
	set( PRE_BUILD_PROJECT_NAME ${PRE_BUILD_PROJECT_NAME_BAK} )
	unset( PROJECT_SOURCE_DIR_BAK )
	unset( PRE_BUILD_PROJECT_NAME_BAK )

	unset( GET_ONLY_INTERNALS )

	FOREACH(filename ${generic_filenames})
		IF(EXISTS "${DEPOT}/${type}/${full}/${filename}")
			ADD_ECHO_TO_TARGET( ${target} "deploy: ${type}/${full}/${filename}" )
		ENDIF(EXISTS "${DEPOT}/${type}/${full}/${filename}")
	ENDFOREACH(filename ${generic_filenames})

	FOREACH(group ${${name}_info_OPTIONS_NAMES})
		FOREACH(opt ${${group}_OPTIONS})
			FOREACH( filebase ${custom_filenames})
				IF(${${group}_USE_${opt}} AND EXISTS "${DEPOT}/${type}/${full}/${filebase}.${group}_${opt}.cmake" )
					ADD_ECHO_TO_TARGET( ${target} "deploy: ${type}/${full}/${filebase}.${group}_${opt}.cmake" )
				ENDIF(${${group}_USE_${opt}} AND EXISTS "${DEPOT}/${type}/${full}/${filebase}.${group}_${opt}.cmake" )
			ENDFOREACH( filebase ${custom_filenames})
			IF(${${group}_USE_${opt}} AND EXISTS "${DEPOT}/${type}/${full}/config.${group}_${opt}.h.cin" )
				ADD_ECHO_TO_TARGET( ${target} "deploy: ${type}/${full}/config.${group}_${opt}.h.cin" )
			ENDIF(${${group}_USE_${opt}} AND EXISTS "${DEPOT}/${type}/${full}/config.${group}_${opt}.h.cin" )
		ENDFOREACH(opt)
	ENDFOREACH(group)

	FOREACH(filename ${${name}_info_SOURCE_FILES} )
		IF(EXISTS "${DEPOT}/${type}/${full}/${filename}" )
			ADD_ECHO_TO_TARGET( ${target} "deploy: ${type}/${full}/${filename}" )
		ENDIF(EXISTS "${DEPOT}/${type}/${full}/${filename}" )
	ENDFOREACH(filename ${${name}_info_SOURCE_FILES} )

endmacro( ADD_FILES_TO_TARGET )

# Deps target
macro( ADD_DEPS_TARGET name components )
	set( target "deps" )
	ADD_CUSTOM_TARGET( "${target}" )

	# Make external list
	LIST( APPEND externals ${${name}_info_EXTERNAL_COMPONENTS} )
	FOREACH(component ${components})
		LIST( APPEND externals ${${component}_info_EXTERNAL_COMPONENTS} )
	ENDFOREACH(component)
	CLEAN_LIST(externals)

	# Show
	set( tmp "${components}" )	# HACK!
	LIST(LENGTH tmp len_i)
	LIST(LENGTH externals len_e)
	ADD_ECHO_TO_TARGET( ${target} "\n" )
	ADD_ECHO_TO_TARGET( ${target} "\tInternals '('${len_i}')': ${components}" )
	ADD_ECHO_TO_TARGET( ${target} "\tExternals '('${len_e}')': ${externals}" )
	ADD_ECHO_TO_TARGET( ${target} "\n" )
endmacro( ADD_DEPS_TARGET )


