-- Test posting an event of class ncl, type selection and action start
require 'testSystem.testSystem'

function handlerOne(evt)
  if evt.class == 'key' then return end
  
  if evt.class == 'ncl' and evt.action == "start" then
    newEvent = {
      class = 'ncl',
      type = 'selection',
      action = 'start',
      label = 'label'
    }
    assertTrue(event.post( 'in', newEvent ), 'No se pudo enviar el evento')
    return true
  end
end

function nclHandler(evt)
  if evt.class == 'key' then return end
  if evt.class == 'ncl' and evt.type == 'presentation' and evt.action == 'stop' then return end
  
  if evt.class == 'ncl' then
    assertEquals(evt.type, 'selection', 'Se esperaba recibir el evento de type selection' )
    assertEquals(evt.action, 'start', 'Se esperaba recibir el evento con action start')
  end
end

event.register(1, handlerOne )
event.register(2, nclHandler )