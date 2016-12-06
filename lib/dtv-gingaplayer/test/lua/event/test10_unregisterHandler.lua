-- Tests that after unregistering an event handler, the handler isn't called any more

require 'testSystem.testSystem'

function handler(evt)
  if evt.class == "key" then return end
  assertTrue(false,"Se esperaba que no se llame al handler despues de eliminar el registro")
end

event.register(handler)
event.unregister(handler)

assertTrue(true)