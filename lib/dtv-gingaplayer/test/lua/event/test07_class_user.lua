-- Test sending an event of class user with two extra fields
require 'testSystem.testSystem'

function handler(evt)
  if evt.class == "key" then return end

  if evt.class == "ncl" and evt.type == "presentation" and evt.action == "start" then 
    eventPosted, errMsg = event.post("in",{ class = "user", a = "aaa", b = "bbb" })
    assertTrue(eventPosted, errMsg)
  end

  if evt.class == "user" then
    assertEquals(evt.class, "user", "Se esperaba que llegase el evento de clase user")
    assertEquals(evt.a, "aaa", "Se esperaba que el campo a tenga el string aaa")
    assertEquals(evt.b, "bbb", "Se esperaba que el campo b tenga el string bbb")
  end
end
  
event.register(handler)