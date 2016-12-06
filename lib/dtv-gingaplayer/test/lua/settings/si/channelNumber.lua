require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.si.channelNumber end )
-- set
assertThrow( function () settings.si.channelNumber = 10 end )