-- Test registering a handler in an explicit position out of order

require 'testSystem.testSystem'
cant = 0

function handlerOne(evt)
  if evt.class == "key" then return end
  errorMsg = "Se esperaba que handlerOne sea la primera funcion en tratar el evento"

  if evt.action == "start" then 
    assertEquals(cant,0,errorMsg) 
  elseif evt.action == "stop" then 
    assertEquals(cant,2,errorMsg) 
  end
  cant = cant + 1
end

function handlerTwo(evt)
  if evt.class == "key" then return end
  errorMsg = "Se esperaba que handlerTwo sea la segunda funcion en tratar el evento"

  if evt.action == "start" then 
    assertEquals(cant,1,errorMsg) 
  elseif evt.action == "stop" then 
    assertEquals(cant,3,errorMsg) 
  end
  cant = cant + 1
end  
  
event.register(3,handlerOne)
event.register(6,handlerTwo)
