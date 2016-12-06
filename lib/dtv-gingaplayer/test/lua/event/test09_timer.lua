require 'testSystem.testSystem'

function check()
  assertTrue(true)
  event.post( { class = "user", action="signal" } )
end  

function handler(evt)
  if evt.class == "key" then return end

  if evt.class == "ncl" and evt.action == "start" then
	event.post( { class = "user", action="wait" } )
	event.timer(10, check)
  end

end

event.register(handler)

