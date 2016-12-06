require 'testSystem.testSystem'

function handlerOne(evt)
  if evt.class == 'key' then return end
  
  if evt.class == 'ncl' and evt.action == "start" then
    newEvent = { class="ncl", type="presentation", action="pause" }
    event.post( 'in', newEvent )
    return true
  end
end

function nclHandler(evt)
  if evt.class == 'key' then return end
  if evt.class == 'ncl' and evt.action == 'stop' then return end
  
  if evt.class == 'ncl' then
    assertEquals(evt.action, 'pause', 'Se esperaba recibir el evento con action pause')
  end
end

event.register(1, handlerOne )
event.register(2, nclHandler )
