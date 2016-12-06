require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.audioType end )
-- set
assertThrow( function () settings.system.audioType = "stereo" end )