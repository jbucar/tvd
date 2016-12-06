-- Tests that when a handler returns true the event isn't handled by other handlers

require 'testSystem.testSystem'

local cant = 0

function handlerZero(evt)
  if evt.class == "key" then return end
  
  if evt.class == "ncl" and evt.action == "start" then
    assertEquals(cant, 0)
  end
  cant = cant + 1
end

function handlerOne(evt)
  if evt.class == "key" then return end
  
  if evt.class == "ncl" and evt.action == "start" then
    assertEquals(cant, 1)
  end
  return true
end

function handlerTwo(evt)
  if evt.class == "key" then return end
  
  assertTrue(false,"Se esperaba que handlerTwo no tratase el evento")
end
  
function handlerThree(evt)
  if evt.class == "key" then return end
  
  assertTrue(false,"Se esperaba que handlerThree no tratase el evento")
end

event.register(handlerZero)
event.register(handlerOne)
event.register(handlerTwo)
event.register(handlerThree)