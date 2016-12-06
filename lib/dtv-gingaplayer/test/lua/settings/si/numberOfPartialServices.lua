require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.si.numberOfPartialServices end )
-- set
assertThrow( function () settings.si.numberOfPartialServices = 10 end )