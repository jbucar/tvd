require 'testSystem.testSystem'
TType = "table"

assertEquals( type(settings.service), TType, "Se esperaba que settings sea una tabla" )

-- override settings.service
assertThrow( function () settings.service = 1 end )
assertEquals( type(settings.service), TType, "Se esperaba que settings sea una tabla" )

-- add test
assertThrow( function () settings.service.myVar = 1 end )
assertEquals( settings.service.myVar, nil )