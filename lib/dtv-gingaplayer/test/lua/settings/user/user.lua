require 'testSystem.testSystem'
TType = "table"

assertEquals( type(settings.user), TType, "Se esperaba que settings.user sea una tabla" )

-- override settings.user
assertThrow( function () settings.user = 1 end )
assertEquals( type(settings.user), TType, "Se esperaba que settings.user sea una tabla" )

-- add test
assertThrow( function () settings.user.myVar = 1 end )
assertEquals( settings.user.myVar, nil )