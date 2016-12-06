require 'testSystem.testSystem'
TType = "table"

assertEquals( type(settings.si), TType, "Se esperaba que settings.si sea una tabla" )

-- override settings.si
assertThrow( function () settings.si = 1 end )
assertEquals( type(settings.si), TType, "Se esperaba que settings.si sea una tabla" )

-- add test
assertThrow( function () settings.si.myVar = 1 end )
assertEquals( settings.si.myVar, nil )