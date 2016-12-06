require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.default.focusBorderTransparency end )
--set
assertThrow( function () settings.default.focusBorderTransparency = 1 end )
-- default value
assertEquals( settings.default.focusBorderTransparency, "0" )
