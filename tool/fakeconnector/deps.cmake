ADD_INTERNAL_COMPONENTS( connector canvas util )

ADD_BOOST_COMPONENTS( system filesystem thread )

find_package( Lua REQUIRED )
ADD_DEPENDENTS( LUA )