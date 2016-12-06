require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.user.genre end )
-- set
assertThrow( function () settings.user.genre = "f" end )
