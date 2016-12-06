-- Tests that when a handler returns true the event isn't handled by other handlers

require 'testSystem.testSystem'

function handlerOne(evt)
  if evt.class == "key" then return end
  
  assertTrue(true)
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

event.register(handlerOne)
event.register(handlerTwo)
event.register(handlerThree)