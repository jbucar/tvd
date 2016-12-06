-- Test calling a timer with 0 milliseconds
require 'testSystem.testSystem'

function callback()
  assertTrue(true)
  event.post( {class = 'user', action = 'signal'} )
end
  
function handler( evt )
  if evt.class == 'key' then return end

  if evt.class == 'ncl' and evt.action == 'start' then
    event.post( {class = 'user', action = 'wait'} )
    event.timer(0, callback)
  end
end

event.register(handler)