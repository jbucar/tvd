-- Test posting an event of class ncl, type atribution and action resume
require 'testSystem.testSystem'

function handlerOne(evt)
  if evt.class == 'key' then return end
  
  if evt.class == 'ncl' and evt.action == "start" then
    newEvent = {
      class = 'ncl',
      type = 'attribution',
      action = 'resume',
      name = 'prueba',
      value = '1',
      label = 'label'
    }
    assertTrue( event.post( 'in', newEvent ), 'No se pudo enviar el evento')
    return true
  end
end

function nclHandler(evt)
  if evt.class == 'key' then return end
  if evt.class == 'ncl' and evt.type == 'presentation' and evt.action == 'stop' then return end
  
  if evt.class == 'ncl' then
    assertEquals(evt.type, 'attribution', 'Se esperaba que el evento sea de type attribution' )
    assertEquals(evt.action, 'resume', 'Se esperaba que el evento tenga action resume')
    assertEquals(evt.name, 'prueba', 'Se esperaba que el evento tenga nombre prueba')
    assertEquals(evt.value, '1', 'Se esperaba que el evento tenga valor 1: result=' .. evt.value )
  end
end

event.register(1, handlerOne )
event.register(2, nclHandler )