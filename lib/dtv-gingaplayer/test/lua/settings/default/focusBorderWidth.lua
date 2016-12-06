require 'testSystem.testSystem'

-- get
assertNoThrow( function () var = settings.default.focusBorderWidth end )
-- set
assertThrow( function () settings.default.focusBorderWidth = 10 end )
