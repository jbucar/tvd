require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.classNumber end )
-- set
assertThrow( function () settings.system.classNumber = 5 end )