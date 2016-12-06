-- Test sending an event of class user without any extra field
require 'testSystem.testSystem'

function handler(evt)
  if evt.class == "key" then return end

  if evt.class == "ncl" and evt.type == "presentation" and evt.action == "start" then 
    eventPosted, errMsg = event.post("in",{ class = "user" })
    assertTrue(eventPosted, errMsg)
  end

  if evt.class == "user" then
    assertEquals(evt.class, "user", "se esperaba que llegase el evento de clase user")
  end
end
  
event.register(handler)