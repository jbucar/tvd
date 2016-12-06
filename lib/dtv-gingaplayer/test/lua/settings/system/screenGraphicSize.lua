require 'testSystem.testSystem'

-- get
assertNoThrow( function () w, h = settings.system.screenGraphicSize end )
-- set
assertThrow( function () settings.system.screenGraphicSize = 10 end )
-- default values
assertTrue( settings.system.screenGraphicSize == "(720, 576)", "Graphic Size expected (720, 576)")
