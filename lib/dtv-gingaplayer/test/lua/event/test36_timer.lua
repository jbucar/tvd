-- Test calling two timers with diferent milliseconds
require 'testSystem.testSystem'

local cant = 0

function callback1()
  assertEquals(cant, 0)
  cant = cant + 1
end

function callback2()
  assertEquals(cant, 1)
  event.post( {class = 'user', action = 'signal'} )
end

function handler( evt )
  if evt.class == 'key' then return end

  if evt.class == 'ncl' and evt.action == 'start' then
    event.post( {class = 'user', action = 'wait'} )
    event.timer(10, callback1)
    event.timer(20, callback2)
  end
end

event.register(handler)