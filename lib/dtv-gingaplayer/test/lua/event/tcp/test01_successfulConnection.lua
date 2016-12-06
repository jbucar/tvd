-- Try to connect to a host
require 'testSystem.testSystem'

function tcp_handler(evt)
	if evt.type == "connect" then
		assertTrue(evt.class == "tcp")
		assertTrue(evt.port == "13")
		assertTrue(evt.host == "127.0.0.1")
		assertTrue(evt.connection ~= nil)
		assertTrue(evt.error == nil)
	end
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="127.0.0.1", port="13"} )
