-- Test posting events of class key, type release and keys 0 to 9, red, green, blue and yellow
require 'testSystem.testSystem'

local i = 1
local buttons = {
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  'RED',
  'GREEN',
  'BLUE',
  'YELLOW'
}

function handlerOne(evt)
  if evt.class == 'key' then return end
  
  if evt.class == 'ncl' and evt.action == "start" then
    for _,button in ipairs(buttons) do
      newEvent = {
	class = 'key',
	type = 'release',
	key = button
      }
      assertTrue(event.post( 'in', newEvent ), 'No se pudo enviar el evento')
    end
    return true
  end
end

function nclHandler(evt)
  if evt.class == 'ncl' then return end
  
  if evt.class == 'key' then
    assertEquals(evt.type, 'release', 'Se esperaba recibir el evento de type press' )
    assertEquals(evt.key, buttons[i] , 'Se esperaba recibir el evento con key ' .. buttons[i] )
    i = i + 1
  end
end

event.register(1, handlerOne )
event.register(2, nclHandler )