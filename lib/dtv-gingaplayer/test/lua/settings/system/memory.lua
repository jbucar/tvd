require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.memory end )
-- set
assertThrow( function () settings.system.memory = 50 end )