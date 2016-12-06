require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.default.focusBorderColor end )
-- set
assertThrow( function () settings.default.focusBorderColor = "white" end )
-- default value
assertEquals( settings.default.focusBorderColor, "white" )
