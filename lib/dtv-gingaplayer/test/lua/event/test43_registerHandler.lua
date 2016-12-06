require 'testSystem.testSystem'

function handlerTwo(evt)
end  

function handlerOne(evt)
  event.register(1, handlerTwo)
  assertEquals(evt.class,"ncl")
end
  
event.register(handlerOne)

