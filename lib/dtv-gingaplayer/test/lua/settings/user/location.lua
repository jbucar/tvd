require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.user.location end )
-- set
assertThrow( function () settings.user.location = "string" end )