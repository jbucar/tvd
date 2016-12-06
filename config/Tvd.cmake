PROJECT( "tvd" )

if( POLICY CMP0054 )
	cmake_policy(SET CMP0011 OLD)
	cmake_policy(SET CMP0054 OLD)
endif()

# Check for host and target system, toolchain, etc
IF (NOT CMAKE_SYSTEM_NAME)
	message(FATAL_ERROR "\n -- \t ERROR: Invalid SYSTEM, you need set it in toolchain file: set(CMAKE_SYSTEM_NAME \"Linux\")\n")
ENDIF (NOT CMAKE_SYSTEM_NAME)

# Add DEPOT/config, DEPOT/config/modules to cmake find modules path
FILE(TO_CMAKE_PATH $ENV{DEPOT} DEPOT)
SET (CONFIG_PATH ${DEPOT}/config)
SET (FIND_PATH ${CONFIG_PATH}/modules )
LIST(APPEND CMAKE_MODULE_PATH ${CONFIG_PATH} ${FIND_PATH})

# Include the Tvd.cmake own macros
INCLUDE(Utils)
INCLUDE(Impl)

# Set verbosity (Necesary for some utils macros)
setIfEmptyEnv(VERBOSITY TVD_VERBOSITY 0)

# Include toolchain file
IF (CMAKE_TOOLCHAIN_FILE)
	INCLUDE_IF_EXIST( ${CMAKE_TOOLCHAIN_FILE} )
ENDIF (CMAKE_TOOLCHAIN_FILE)

# Use ccache if requested
IF("$ENV{USE_CCACHE}" STREQUAL "1")
	SET(CMAKE_C_COMPILER_ARG1 ${CMAKE_C_COMPILER} )
	SET(CMAKE_CXX_COMPILER_ARG1 ${CMAKE_CXX_COMPILER} )
	SET(CMAKE_C_COMPILER   "ccache" )
	SET(CMAKE_CXX_COMPILER "ccache" )
ENDIF("$ENV{USE_CCACHE}" STREQUAL "1")

# Set platform name if empty
setPlatformName()

# Include Global config
INCLUDE_COMPONENTS( "${CONFIG_PATH}/target/SetupGlobal" )
# Include User config
INCLUDE_IF_EXIST(${SETUP_USER})

# Fix paths
FILE(TO_CMAKE_PATH ${TVD_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
FILE(TO_CMAKE_PATH ${TVD_CONFIG_INSTALL_PREFIX} CMAKE_CONFIG_INSTALL_PREFIX)

# Setup cmake find prefix path (for FindPackage)
IF (PLATFORM_FIND_ROOT_PATH)
	LIST(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_INSTALL_PREFIX} ${PLATFORM_FIND_ROOT_PATH} )
ELSE()
	LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX} )
ENDIF()

# Setup list of required variables for the parse_arguments()
SET(PRE_BUILD_VARIABLES "PROJECT_NAME;PROJECT_DESCRIPTION;PROJECT_TESTS;PROJECT_CONTACT")

# Add a library target
MACRO (Add_Lib)
	parse_arguments( PRE_BUILD "${PRE_BUILD_VARIABLES}" "" ${ARGN} )
	DoBuild( "lib" )
	INSTALL_HEADERS_WITH_DIRECTORY( "include/${PROJECT_PREFIX}/${PRE_BUILD_PROJECT_NAME}" "src/|include/" ${PRE_BUILD_PROJECT_NAME}_info_HEADER_FILES )
ENDMACRO (Add_Lib)

# Add a binary target
MACRO (Add_Bin)
	parse_arguments( PRE_BUILD "${PRE_BUILD_VARIABLES};PROJECT_SERVICE;SUBSYSTEM;BIN_TYPE;BIN_DESTINATION;ICON_NAME;LICENSE_FILE" "" ${ARGN} )
	DoBuild( "tool" )
ENDMACRO (Add_Bin)

# Add documentation
MACRO (Add_Doc)
	parse_arguments( PRE_DOC "NAME;TEX;SOURCE;DESTINATION" "" ${ARGN} )
	MAKE_DOC( ${PRE_DOC_NAME} ${PRE_DOC_TEX} ${PRE_DOC_SOURCE} ${PRE_DOC_DESTINATION} )
ENDMACRO (Add_Doc)

# Add Doxygen documentation
MACRO (Add_Doxygen)
	parse_arguments( PRE_DOC "NAME;SOURCE;DESTINATION" "" ${ARGN} )
	DO_DOXYGEN( ${PRE_DOC_NAME} ${PRE_DOC_SOURCE} ${PRE_DOC_DESTINATION} )
ENDMACRO (Add_Doxygen)

macro(ADD_DEPENDENTS)
	# Only add when no only internals is requerid
	if(NOT GET_ONLY_INTERNALS)
		ADD_DEPENDENTS_IMPL(${ARGN})
	endif(NOT GET_ONLY_INTERNALS)
endmacro(ADD_DEPENDENTS)

MACRO(ADD_HEADERS)
	LIST( APPEND HEADER_FILES ${ARGN} )
ENDMACRO(ADD_HEADERS)

MACRO(ADD_SOURCES)
	LIST( APPEND SOURCE_FILES ${ARGN} )
ENDMACRO(ADD_SOURCES)

MACRO(ADD_BOOST_COMPONENTS )
	IF(NOT GET_ONLY_INTERNALS)
		ADD_BOOST_COMPONENTS_IMPL("" ${ARGN})
	ENDIF(NOT GET_ONLY_INTERNALS)
ENDMACRO(ADD_BOOST_COMPONENTS )

MACRO(ADD_INTERNAL_COMPONENTS)
	ADD_INTERNAL_COMPONENTS_IMPL("" ${ARGN})
ENDMACRO(ADD_INTERNAL_COMPONENTS)

macro(CHECK_OPTION opt val)
	if(NOT (${${opt}} EQUAL ${val}))
		message(FATAL_ERROR "\n -- \t ERROR: ${opt} is not defined with ${val}.\n")
	endif(NOT (${${opt}} EQUAL ${val}))
endmacro(CHECK_OPTION opt val)

macro(DEF_OPTIONS group)
	STRING(TOUPPER ${group} _GROUP)
	SET(${_GROUP}_OPTIONS "")
	#collecting args. Stop at "DEFAULT" value.
	#collecting arg_values. Iterate in ${ARGN} once.
	set(args "")
	set(args_values "")

	set(collector_list args)
	foreach(opt ${ARGN})
		STRING(TOUPPER ${opt} arg)
		if(${arg} STREQUAL "DEFAULT")
			set(collector_list args_values)
		else()
			list(APPEND ${collector_list} ${arg})
		endif(${arg} STREQUAL "DEFAULT")
	endforeach(opt ${ARGN})

	PARSE_ARGUMENTS(${_GROUP} "${args}" "" ${args_values})

	#check values of args.
	foreach(arg ${args})
		SetIfEmpty(${_GROUP}_${arg} OFF) #aca todas los ARGS van a tener definido un valor=(ON,AUTO,OFF).
		SetIfEmpty(${_GROUP}_USE_${arg} "${${_GROUP}_${arg}}" )

		if(${_GROUP}_USE_${arg})
			LIST(APPEND ${_GROUP} ${arg} )
			IF("${${_GROUP}_USE_${arg}}" STREQUAL "ON")
				set( ${_GROUP}_USE_${arg} 1)
				set( ${arg}_REQUIRED 1 )
			ELSE("${${_GROUP}_USE_${arg}}" STREQUAL "ON")
				set( ${arg}_REQUIRED 0 )
			ENDIF("${${_GROUP}_USE_${arg}}" STREQUAL "ON")
		else(${_GROUP}_USE_${arg})
			set( ${_GROUP}_USE_${arg} 0)
		endif(${_GROUP}_USE_${arg})
	endforeach(arg ${args})

	SET( ${_GROUP}_OPTIONS ${args})

	CLEAN_LIST( ${_GROUP} )
	LIST(APPEND OPTIONS_NAMES ${_GROUP})
endmacro(DEF_OPTIONS group)

macro( create_target type name )
	message( STATUS "Create target begin: ${type} ${name}" )

	# Get source directory
	GET_SOURCE_DIR( ${type} ${name} src )

	# Get build list
	GET_PROJECT_BUILD_ORDER( ${type} ${name} ${src} int_comps 1 )

	GENERATE_OPTIONS( ${name} "${int_comps}")

	# Add target for all depends
	FOREACH(lib ${int_comps})
		# Add target for build
		ADD_TARGET( ${lib} "lib" 0 "${${lib}_info_INTERNAL_COMPONENTS}" )
	ENDFOREACH(lib)

	# Add target for the project
	ADD_TARGET( ${name} ${type} 1 "${int_comps}" )

	# Add extra targets
	ADD_EXTRA_TARGETS( ${name} ${type} "${int_comps}" )
	ADD_OPTIONS_TARGET( ${name} "${components}" )
	# HACK: ADD_FILES_TARGET only here to prevent double inclusion of sources.*.cmake
	ADD_FILES_TARGET( ${name} ${type} "${int_comps}" )

	message( STATUS "Create target end: ${type} ${name}" )
endmacro( create_target )
