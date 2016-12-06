# Packages
ADD_INTERNAL_COMPONENTS( luaz zapper mpegparser connector canvas util )
ADD_BOOST_COMPONENTS( filesystem system date_time )

DEF_OPTIONS(ZAMBA_SCRIPTS AML)

find_package( Lua REQUIRED )
ADD_DEPENDENTS(LUA)
