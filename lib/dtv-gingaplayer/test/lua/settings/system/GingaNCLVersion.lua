require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.gingaNCLVersion end )
-- set
assertThrow( function () settings.system.gingaNCLVersion = "string" end )

print(settings.system.gingaNCLVersion )

pos = string.find( settings.system.gingaNCLVersion, "Ginga.ar" )

-- default values
assertTrue( pos ~= nil, "GingaNCL version expected: Ginga.ar X.X.X")


