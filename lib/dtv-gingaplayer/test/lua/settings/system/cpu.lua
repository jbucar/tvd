require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.cpu end )
-- set
assertThrow( function () settings.system.cpu = 100 end )