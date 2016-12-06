require 'testSystem.testSystem'

function handler(evt)
	assertTrue(evt.class == "ncl")
end
  
event.register(handler, "ncl")

