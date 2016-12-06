require 'testSystem.testSystem'

local cant = 0

function my_handler(evt)
  if evt.class ~= 'ncl' then return end
  if evt.action ~= 'start' then return end

  assertEquals(evt.type, 'presentation', 'Se esperaba que el evento sea de type presentation' )
  if cant == 1 then
	assertEquals(evt.label, 'basic', 'Se esperaba que el evento tenga label basic') 
  end

  cant = cant + 1
end

event.register(my_handler)

