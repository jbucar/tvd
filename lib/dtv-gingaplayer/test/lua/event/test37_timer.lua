-- Test calling two timers and unregistering one
require 'testSystem.testSystem'

function callback1()
  assertTrue(false)
end

function callback2()
  assertTrue(true)
  event.post( {class = 'user', action = 'signal'} )
end

function handler( evt )
  if evt.class == 'key' then return end

  if evt.class == 'ncl' and evt.action == 'start' then
    event.post( {class = 'user', action = 'wait'} )
    unreg1 = event.timer(100, callback1)
    unreg2 = event.timer(200, callback2)
    unreg1()
  end
end

event.register(handler)