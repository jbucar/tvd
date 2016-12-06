-- Tests unregistering a handler twice

require 'testSystem.testSystem'

function handler(evt)
  if evt.class == "key" then return end

  if evt.action == "start" then 
    assertTrue(false, "Se esperaba que el handler no sea llamado despues de cancelar su registracion")
  end
end

event.register(handler)
event.unregister(handler)
event.unregister(handler)

assertTrue(true)