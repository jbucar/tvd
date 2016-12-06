require 'testSystem.testSystem'

function handler(evt)
	assertTrue(evt.class == "key")
end
  
event.register(handler, "key")
event.post( 'in', { class = 'key', type = 'press', key = "7" })
      

