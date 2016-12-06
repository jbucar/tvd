require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.luaVersion end )
-- set
assertThrow( function () settings.system.luaVersion = "string" end )
-- default values
assertTrue( settings.system.luaVersion == "5.1", "LUA version expected 5.1")
