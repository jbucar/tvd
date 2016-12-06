require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.nclVersion end )
-- set
assertThrow( function () settings.system.nclVersion = "string" end )
-- default values
assertTrue( settings.system.nclVersion == "3.0", "NCL version expected 3.0")
