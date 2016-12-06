require 'testSystem.testSystem'

function handler(evt)
	assertTrue(evt.class == "ncl")
	assertTrue(evt.class == "presentation")
end
  
assertThrow(function() event.register(handler, "ncl", "presentation") end)

