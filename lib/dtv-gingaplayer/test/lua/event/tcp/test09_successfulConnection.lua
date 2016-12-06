-- Try to connect to a host
require 'testSystem.testSystem'

function tcp_handler(evt)
	if evt.type == "connect" then
		assertTrue(evt.class == "tcp")
		assertTrue(evt.port == "80")
		assertTrue(evt.host == "www.google.com")
		assertTrue(evt.connection ~= nil)
		assertTrue(evt.error == nil)
	end
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="www.google.com", port="80"} )
