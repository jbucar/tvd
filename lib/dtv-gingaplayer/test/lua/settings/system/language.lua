require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.system.language end )
-- set
assertThrow( function () settings.system.language = "ISO 639-1" end )