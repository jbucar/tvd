CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

# Connector keep alive interval
SetIfEmpty(CONNECTOR_KEEP_ALIVE_INTERVAL 500)

# Connector keep alive max retry
SetIfEmpty(CONNECTOR_KEEP_ALIVE_MAX_RETRY 5)

# Connector connect timeout
SetIfEmpty(CONNECTOR_CONNECT_TIMEOUT 10000)

# Connector connect retry timeout
SetIfEmpty(CONNECTOR_CONNECT_TIMEOUT_RETRY 1000)

# Connector connect max retry timeout
SetIfEmpty(CONNECTOR_CONNECT_TIMEOUT_MAX_RETRY 10)

SetIfEmpty(CONNECTOR_GINGA_VERSION \"\" )

Add_Lib (
	PROJECT_NAME "connector"
	PROJECT_DESCRIPTION "This library connect zapper with ginga." 
	PROJECT_TESTS "test"
)


