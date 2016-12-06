require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.subtitle end )
-- set
assertThrow( function () settings.system.subtitle = "ISO 639-1" end )