-- Try to connect to a host and then disconnect
require 'testSystem.testSystem'

function tcp_handler(evt)
	assertFalse(evt.type == "data")
	assertTrue(evt.class == "tcp")
	assertTrue(evt.type == "connect")
	assertTrue(evt.port == "13")
	assertTrue(evt.host == "127.0.0.1")
	assertTrue(evt.connection ~= nil)
	assertTrue(evt.error == nil)

	if evt.type == "connect" then
		event.post( 'in', {class="tcp", type="disconnect", connection=evt.connection } )
		assertThrow(function() event.post( 'in', {class="tcp", type="data", connection=id, value=value} ) end )
	end
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="127.0.0.1", port="13"} )