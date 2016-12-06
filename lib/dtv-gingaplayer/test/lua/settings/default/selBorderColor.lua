require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.default.selBorderColor end )
-- set
assertThrow( function () settings.default.selBorderColor = "white" end )
-- default values
assertEquals( settings.default.selBorderColor, "white" )