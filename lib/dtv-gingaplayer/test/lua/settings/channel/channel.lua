require 'testSystem.testSystem'
TType = "table"

assertEquals( type(settings.channel), TType, "Se esperaba que settings.channel sea una tabla" )

-- override settings.channel
assertThrow( function () settings.channel = 1 end )
assertEquals( type(settings.channel), TType, "Se esperaba que settings.channel sea una tabla" )

-- add test
assertThrow( function () settings.channel.myVar = 1 end )
assertEquals( settings.channel.myVar, nil )