require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.si.numberOfServices end )
-- set
assertThrow( function () settings.si.numberOfServices = 10 end )