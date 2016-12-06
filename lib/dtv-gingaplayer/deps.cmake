ADD_BOOST_COMPONENTS( system filesystem thread )

ADD_INTERNAL_COMPONENTS( connector canvas util )

find_package( Lua REQUIRED )
find_package( CURL REQUIRED )
ADD_DEPENDENTS( LUA CURL )
