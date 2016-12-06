# Libraries
ADD_INTERNAL_COMPONENTS( zapper mpegparser canvas util )
ADD_BOOST_COMPONENTS( system filesystem thread date_time )

find_package( Lua REQUIRED )
ADD_DEPENDENTS(LUA)

