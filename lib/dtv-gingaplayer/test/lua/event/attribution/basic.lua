require 'testSystem.testSystem'

function my_handler(evt)
  if evt.class == "key" then return end
  if evt.class == "ncl" and evt.type == "presentation" then return end

  assertEquals(evt.class,"ncl")
  assertEquals(evt.type, 'attribution', 'Se esperaba que el evento sea de type attribution' )
  assertEquals(evt.action, 'start', 'Se esperaba que el evento tenga action start')
  assertEquals(evt.name, 'basic', 'Se esperaba que el evento tenga nombre prueba')
  assertEquals(evt.value, '1', 'Se esperaba que el evento tenga valor 1')
end

event.register(my_handler)

