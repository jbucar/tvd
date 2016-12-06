require 'testSystem.testSystem'
TType = "table"

assertEquals( type(settings.shared), TType, "Se esperaba que settings.shared sea una tabla" )

-- override settings.shared
assertThrow( function () settings.shared = 1 end )
assertEquals( type(settings.shared), TType, "Se esperaba que settings.shared sea una tabla" )

-- add test
assertThrow( function () settings.shared.myVar = 1 end )
assertEquals( settings.shared.myVar, nil )