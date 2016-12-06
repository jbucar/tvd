-- Test that a handler registered twice is only registered once

require 'testSystem.testSystem'
cant = 0

function handler(evt)
  if evt.class == "key" then return end
  
  if evt.action == "start" then 
    assertEquals(cant,0,"Se espera que se llame una sola vez al handler") 
    cant = cant + 1
  end
end

event.register(handler)
event.register(handler)
