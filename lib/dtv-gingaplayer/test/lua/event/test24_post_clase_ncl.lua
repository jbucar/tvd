-- Test posting an event of class ncl, type attribution and action abort
require 'testSystem.testSystem'

function handlerOne(evt)
  if evt.class == 'key' then return end
  
  if evt.class == 'ncl' and evt.action == "start" then
    newEvent = {
      class = 'ncl',
      type = 'attribution',
      action = 'abort',
      name = 'prueba',
      value = '1',
      label = 'label'
    }
    res, error = event.post( 'in', newEvent )
    assertTrue(res, 'No se pudo enviar el evento - ' .. tostring(error) )
    return true
  end
end

function nclHandler(evt)
  if evt.class == 'key' then return end
  if evt.class == 'ncl' and evt.type == 'presentation' and evt.action == 'stop' then return end
  
  if evt.class == 'ncl' then
    assertEquals(evt.type, 'attribution', 'Se esperaba que el evento sea de type attribution' )
    assertEquals(evt.action, 'abort', 'Se esperaba que el evento tenga action abort')
    assertEquals(evt.name, 'prueba', 'Se esperaba que el evento tenga nombre prueba')
    assertEquals(evt.value, '1', 'Se esperaba que el evento tenga valor 1')
  end
end

event.register(1, handlerOne )
event.register(2, nclHandler )