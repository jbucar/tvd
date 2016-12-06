require 'testSystem.testSystem'

-- get
assertNoThrow( function () w, h = settings.system.screenSize end )
-- set
assertThrow( function () settings.system.screenSize = 10 end )
-- default values
assertTrue( settings.system.screenSize == "(720, 576)", "Screen Size expected (720, 576)")
