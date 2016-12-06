CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

SetIfEmpty( GINGA_CONNECTOR_SOCKET \"/tmp/gingaconnector.socket\" )
SetIfEmpty( GINGAPLAYER_LUA_PERSISTENT_FILE "\"\"" )

Add_Lib (
	PROJECT_NAME "gingaplayer"
	PROJECT_DESCRIPTION "This is the Ginga Player library project."
	PROJECT_TESTS "test"
)

