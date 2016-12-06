-- Test that handlers are called in the order specified

require 'testSystem.testSystem'
pos = 1

function handlerOne(evt)
  if evt.class == "key" then return end
  errorMsg = "Se esperaba que handlerOne sea la primera funcion en tratar el evento"
  
  if evt.action == "start" then 
    assertEquals(pos,1,errorMsg) 
  elseif evt.action == "stop" then 
    assertEquals(pos,3,errorMsg) 
  end
  pos = pos + 1
end

function handlerTwo(evt)
  if evt.class == "key" then return end
  errorMsg = "Se esperaba que handlerTwo sea la segunda funcion en tratar el evento"
  
  if evt.action == "start" then 
    assertEquals(pos,2,errorMsg) 
  elseif evt.action == "stop" then 
    assertEquals(pos,4,errorMsg) 
  end
  pos = pos + 1
end 
  
event.register(1,handlerOne)
event.register(2,handlerTwo)
