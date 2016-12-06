require 'testSystem.testSystem'
TType = "table"

assertEquals( type(settings.system), TType, "Se esperaba que settings.system sea una tabla" )

-- override settings.user
assertThrow( function () settings.system = 1 end )
assertEquals( type(settings.system), TType, "Se esperaba que settings.system sea una tabla" )

-- add test
assertThrow( function () settings.system.myVar = 1 end )
assertEquals( settings.system.myVar, nil )