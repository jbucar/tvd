-- Test posting an event of class ncl, type presentation and action abort
require 'testSystem.testSystem'

function handlerOne(evt)
  if evt.class == 'key' then return end
  
  if evt.class == 'ncl' and evt.action == "start" then
    newEvent = {
      class = 'ncl',
      type = 'presentation',
      action = 'abort',
      label = 'label'
    }
    res, error = event.post( 'in', newEvent )
    assertTrue(res, 'No se pudo enviar el evento - ' .. tostring(error) )
    return true
  end
end

function nclHandler(evt)
  if evt.class == 'key' then return end
  if evt.class == 'ncl' and evt.action == 'stop' then return end
  
  if evt.class == 'ncl' then
    assertEquals(evt.type, 'presentation', 'Se esperaba recibir el evento de type presentation' )
    assertEquals(evt.action, 'abort', 'Se esperaba recibir el evento con action abort')
  end
end

event.register(1, handlerOne )
event.register(2, nclHandler )