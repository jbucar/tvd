require 'testSystem.testSystem'

function my_handler(evt)
  if evt.class == "key" then return end
  assertEquals(evt.class,"ncl")
  if evt.class == "ncl" and evt.action == "stop" then
	event.unregister(my_handler)
  end
end

event.register(1,my_handler)

