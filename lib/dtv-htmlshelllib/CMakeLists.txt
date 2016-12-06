CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

Add_Lib (
	PROJECT_NAME "htmlshelllib"
	PROJECT_DESCRIPTION "Base library for building extensions for HtmlShell tool"
	PROJECT_TESTS "test"
)
