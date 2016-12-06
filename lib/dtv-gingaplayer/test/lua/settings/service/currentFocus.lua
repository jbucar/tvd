require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.service.currentFocus end )
-- set
assertThrow( function () settings.service.currentFocus = 10 end )