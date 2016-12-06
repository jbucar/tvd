##########################################################################
# Boost Utilities                                                        #
##########################################################################
# Copyright (C) 2007 Douglas Gregor <doug.gregor@gmail.com>              #
# Copyright (C) 2007 Troy Straszheim                                     #
#                                                                        #
# Distributed under the Boost Software License, Version 1.0.             #
# See accompanying file LICENSE_1_0.txt or copy at                       #
#   http://www.boost.org/LICENSE_1_0.txt                                 #
##########################################################################
# Macros in this module:                                                 #
#                                                                        #
#   list_contains: Determine whether a string value is in a list.        #
#                                                                        #
#   parse_arguments: Parse keyword arguments for use in other macros.    #
##########################################################################

# This utility macro determines whether a particular string value
# occurs within a list of strings:
#
#  list_contains(result string_to_find arg1 arg2 arg3 ... argn)
# 
# This macro sets the variable named by result equal to TRUE if
# string_to_find is found anywhere in the following arguments.
macro(list_contains var value)
  set(${var})
  foreach (value2 ${ARGN})
    if (${value} STREQUAL ${value2})
      set(${var} TRUE)
    endif (${value} STREQUAL ${value2})
  endforeach (value2)
endmacro(list_contains)

# The PARSE_ARGUMENTS macro will take the arguments of another macro and
# define several variables. The first argument to PARSE_ARGUMENTS is a
# prefix to put on all variables it creates. The second argument is a
# list of names, and the third argument is a list of options. Both of
# these lists should be quoted. The rest of PARSE_ARGUMENTS are
# arguments from another macro to be parsed.
# 
#     PARSE_ARGUMENTS(prefix arg_names options arg1 arg2...) 
# 
# For each item in options, PARSE_ARGUMENTS will create a variable with
# that name, prefixed with prefix_. So, for example, if prefix is
# MY_MACRO and options is OPTION1;OPTION2, then PARSE_ARGUMENTS will
# create the variables MY_MACRO_OPTION1 and MY_MACRO_OPTION2. These
# variables will be set to true if the option exists in the command line
# or false otherwise.
# 
# For each item in arg_names, PARSE_ARGUMENTS will create a variable
# with that name, prefixed with prefix_. Each variable will be filled
# with the arguments that occur after the given arg_name is encountered
# up to the next arg_name or the end of the arguments. All options are
# removed from these lists. PARSE_ARGUMENTS also creates a
# prefix_DEFAULT_ARGS variable containing the list of all arguments up
# to the first arg_name encountered.
MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})
    LIST_CONTAINS(is_arg_name ${arg} ${arg_names})
    IF (is_arg_name)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name)
      LIST_CONTAINS(is_option ${arg} ${option_names})
      IF (is_option)
      SET(${prefix}_${arg} TRUE)
      ELSE (is_option)
      SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option)
    ENDIF (is_arg_name)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

#sets correctly the verbosity param for each lib/tool compile. (build/CmakeLists.txt).
macro(PARSE_VERBOSITY verb param)
	if(${verb} EQUAL 3)
		set(${param} "--debug-output")
	elseif(${verb} EQUAL 4)
		set(${param} "--trace")
	endif(${verb} EQUAL 3)
endmacro(PARSE_VERBOSITY verb)

MACRO(SetIfEmpty name value)
	IF(NOT DEFINED ${name} OR "${name}" STREQUAL "")
		SET(${name} ${value})
	ENDIF(NOT DEFINED ${name} OR "${name}" STREQUAL "")
ENDMACRO(SetIfEmpty)

MACRO(setIfEmptyEnv name env_value value)
	IF (NOT $ENV{${env_value}} STREQUAL "")
		SET( ${name} $ENV{${env_value}})
	ENDIF ()
	SetIfEmpty( ${name} ${value} )
ENDMACRO(setIfEmptyEnv)

MACRO(InitVar nameVar initValue defaultValue)
	IF("${initValue}" STREQUAL "")
		SET( ${nameVar} ${defaultValue} )
	ELSE()
		SET( ${nameVar} ${initValue} )
	ENDIF()
ENDMACRO(InitVar)

MACRO(INSTALL_HEADERS_WITH_DIRECTORY DESTINATION HEADER_REMOVE HEADER_LIST)
	FOREACH(HEADER ${${HEADER_LIST}})
		STRING(REGEX MATCH "(.*)[/\\]" DIR ${HEADER}i )
		STRING(REGEX REPLACE "${HEADER_REMOVE}" "" DIR "${DIR}" )
		INSTALL(FILES ${HEADER} DESTINATION ${DESTINATION}/${DIR})
	ENDFOREACH(HEADER)
ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

MACRO(OPTION_IF_EMPTY VAR DESCRIPTION)
	IF(NOT DEFINED ${VAR})	
		OPTION(${VAR} "${DESCRIPTION}" ${ARGN} )
	ENDIF(NOT DEFINED ${VAR})	
ENDMACRO(OPTION_IF_EMPTY)

FIND_PROGRAM(SVN_EXECUTABLE svn DOC "subversion command line client")

MACRO(GET_SVN_REVISION dir var)
  EXECUTE_PROCESS(COMMAND ${SVN_EXECUTABLE} info ${dir}
    OUTPUT_VARIABLE ${var}
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  STRING(REGEX REPLACE "^(.*\n)?Revisi?.?.n: ([^\n]+).*"
    "\\2" ${var} "${${var}}")
#	SET("${${var}}" ${var})
ENDMACRO(GET_SVN_REVISION)

FIND_PROGRAM(GIT_EXECUTABLE git DOC "git command line client")

MACRO(GET_GIT_REVISION tag build commit )
	EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} --git-dir=$ENV{DEPOT}/.git describe --tags
		OUTPUT_VARIABLE ${commit}
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	IF("${${commit}}" STREQUAL "")
		set( ${commit} "r2.2-0-0" )
	ENDIF()
	STRING(FIND "${${commit}}" "-" out)
	IF ("${out}" STREQUAL "-1")
		set( ${commit} "${${commit}}-0-0" )
	endif()
	STRING(REGEX REPLACE "^([0-9 a-z A-Z .]+)\\-.*" "\\1" ${tag} "${${commit}}")
	STRING(REGEX REPLACE ".*\\-([0-9]+).*" "\\1" ${build} "${${commit}}")
ENDMACRO(GET_GIT_REVISION)

MACRO(GET_BUILD_INFO var)
	GET_SVN_REVISION (${PROJECT_SOURCE_DIR} svnRevision)
	EXECUTE_PROCESS( COMMAND hostname OUTPUT_VARIABLE myhostname OUTPUT_STRIP_TRAILING_WHITESPACE)
	SET(${var} \"\\nHOST:\ ${myhostname}\\nSVN\ REVISION:\ ${svnRevision}\\nBUILD_TYPE:\ ${CMAKE_BUILD_TYPE}\\nSHARED:\ ${BUILD_SHARED}\")
ENDMACRO(GET_BUILD_INFO)

MACRO(INCLUDE_IF_EXIST file)
	SET(FILE_INCLUDED 0)
	IF(EXISTS ${file})
		if(${VERBOSITY} GREATER 0)
			MESSAGE( STATUS "Found file: ${file}" )
		endif(${VERBOSITY} GREATER 0)
		INCLUDE(${file})
		SET(FILE_INCLUDED 1)
	ENDIF(EXISTS ${file})
ENDMACRO(INCLUDE_IF_EXIST)

MACRO(SET_IF_EQUAL var v1 v2)
SET(${var} 0)
IF(${v1} STREQUAL ${v2})
	SET(${var} 1)
ENDIF()
ENDMACRO(SET_IF_EQUAL)

MACRO( COPY_AND_CLEAN_VARIABLE out var )
	LIST( APPEND ${out}_${var} ${${var}} )
	CLEAN_LIST( ${out}_${var} )
	UNSET( ${var} )
ENDMACRO( COPY_AND_CLEAN_VARIABLE out var )

MACRO( CLEAN_LIST list )
	IF (${list})
		LIST( REMOVE_DUPLICATES ${list} )
	ENDIF (${list})
ENDMACRO( CLEAN_LIST list )

MACRO( PRINT_VARIABLES )
	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
      message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
ENDMACRO( PRINT_VARIABLES )

MACRO(SET_OPTION _group _var)
	STRING(LENGTH "${_var}" LEN)
	IF(${LEN} GREATER 0)
	    STRING(TOUPPER ${_var} var)
	    STRING(TOUPPER ${_group} group)

		FOREACH(opt ${${group}_OPTIONS})
			SET(${group}_USE_${opt} 0)
		ENDFOREACH(opt)
		SET(${group}_USE_${var} 1)
	ENDIF()
ENDMACRO(SET_OPTION)


MACRO (FIND_DEPENDANTS  destlist names)

	FOREACH( dep ${names} )
		SET (TMP_LIBRARY_${dep} "NOTFOUND" CACHE INTERNAL "Cleared" FORCE) # Clear any stale value, if we got here, we need to find it again
		IF (WIN32)
			SET (libname lib${dep}) #windows expects "libfoo", linux expects "foo"
		ELSE (WIN32)
			SET (libname ${dep})
		ENDIF (WIN32)

		FIND_LIBRARY (TMP_LIBRARY_${dep} NAMES ${libname} PATHS /usr/local /usr/X11 /usr PATH_SUFFIXES lib64 lib)
		SET (${destlist}_${dep} "${TMP_LIBRARY_${dep}}")
		MARK_AS_ADVANCED (TMP_LIBRARY_${dep})

		LIST (APPEND ${destlist} ${${destlist}_${dep}}) 
	ENDFOREACH( dep ${names} )

ENDMACRO (FIND_DEPENDANTS  destlist names)

MACRO (ENV2LIST env_var result_list)
	set( env_value "$ENV{${env_var}}" )
	IF(NOT "${env_value}" STREQUAL "")
		STRING(CONFIGURE "${env_value}" env_expanded @ONLY )
		STRING(REPLACE " " ";" ${result_list} ${env_expanded} )
		UNSET(env_expanded)
	ENDIF(NOT "${env_value}" STREQUAL "")
	unset(env_value)
ENDMACRO (ENV2LIST env_var result_list)
