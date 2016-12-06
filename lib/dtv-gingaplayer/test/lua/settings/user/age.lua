require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.user.age end )
-- set
assertThrow( function () settings.user.age = 10 end )