-- Test registering two handlers. The firstone without position and the second one
-- on an explicit position

require 'testSystem.testSystem'
cant = 0

function handlerOne(evt)
  if evt.class == "key" then return end
  
  if evt.action == "start" then
    assertTrue(false,"Se espera que el segundo handler reemplaze al primero")
  end
end

function handlerTwo(evt)
  if evt.class == "key" then return end
  
  if evt.action == "start" then
    assertEquals(cant,0) 
    cant = cant + 1
  end
end

function testResult(evt)
  if evt.class ~= "ncl" then return end
  
  if evt.action == "stop" then
    assertEquals(cant,1,"Se espera que el segundo handler reemplaze al primero") 
  end
end

event.register(handlerOne)
event.register(1,handlerTwo)
event.register(2,testResult)