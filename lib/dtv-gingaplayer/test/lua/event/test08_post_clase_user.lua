require 'testSystem.testSystem'

function handler(evt)
  if evt.class == "key" then return end

  if evt.class == "ncl" and evt.action == "start" then
	event.post( { class = "user", action="test", value="1" } )
  end

  if evt.class == "user" and evt.action == "test" and evt.value == "1" then
    assertTrue(true)
  end
end

event.register(handler)