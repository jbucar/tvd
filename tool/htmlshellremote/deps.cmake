Find_Package( Protobuf REQUIRED )
ADD_DEPENDENTS( PROTOBUF )

ADD_BOOST_COMPONENTS( system thread )
ADD_INTERNAL_COMPONENTS( node util htmlshelllib htmlshellsdk )
