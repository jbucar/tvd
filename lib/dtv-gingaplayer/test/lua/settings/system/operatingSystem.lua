require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.operatingSystem end )
-- set
assertThrow( function () settings.system.operatingSystem = "string" end )