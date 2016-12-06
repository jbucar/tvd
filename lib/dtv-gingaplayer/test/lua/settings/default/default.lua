require 'testSystem.testSystem'
TType = "table"

assertEquals( type(settings.default), TType, "Se esperaba que settings.default sea una tabla" )

-- override settings.default
assertThrow( function () settings.default = 1 end )
assertEquals( type(settings.default), TType, "Se esperaba que settings.default sea una tabla" )

-- add test
assertThrow( function () settings.default.myVar = 1 end )
assertEquals( settings.default.myVar, nil )