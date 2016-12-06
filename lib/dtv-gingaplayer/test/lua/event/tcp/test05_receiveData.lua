--	Try to receive data from a service
require 'testSystem.testSystem'
local id = -1

function tcp_handler(evt)
	if (evt.type == "connect") then
		assertTrue(evt.class == "tcp")
		assertTrue(evt.port == "13")
		assertTrue(evt.host == "127.0.0.1")
		assertTrue(evt.connection ~= nil)
		assertTrue(evt.error == nil)

		id = evt.connection;
	else
		assertTrue(id ~= -1)
		assertTrue(evt.class == "tcp")
		assertTrue(evt.type == "data")
		assertTrue(evt.connection == id)
		assertTrue(evt.value ~= nil)
	end
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="127.0.0.1", port="13"} )

