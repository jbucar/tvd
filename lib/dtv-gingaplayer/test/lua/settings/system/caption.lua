require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.caption end )
-- set
assertThrow( function () settings.system.caption = "ISO 639-1" end )