require 'testSystem.testSystem'

function timeout()
	assertTrue(true)
	event.post( {class = 'user', action = 'signal'} )
end

function handler(evt)
	if evt.class == 'key' then
		event.post( {class = 'user', action = 'wait'} )
		event.timer(50, timeout)
	end
end

event.register(handler)
event.post( 'in', { class = 'key', type = 'press', key = "1" })
