-- Gets the uptime, calls a timer with 10 milliseconds and
-- finally test that the actual uptime is greater than the oldone
require 'testSystem.testSystem'

local oldtime

function callback()
  newtime = event.uptime()

  assertTrue( oldtime < newtime )
  event.post( {class = 'user', action = 'signal'} )
end
  
function handler( evt )
  if evt.class == 'key' then return end

  if evt.class == 'ncl' and evt.action == 'start' then
    oldtime = event.uptime()
    
    event.post( {class = 'user', action = 'wait'} )
    event.timer(1000, callback)
  end
end

event.register(handler)