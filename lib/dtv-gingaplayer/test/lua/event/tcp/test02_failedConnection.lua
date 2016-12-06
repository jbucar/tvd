--	Try to connect to a non valid port
require 'testSystem.testSystem'

function tcp_handler(evt)
	assertTrue(evt.class == "tcp")
	assertTrue(evt.type == "connect")
	assertTrue(evt.port == "808080")
	assertTrue(evt.host == "127.0.0.1")
	assertTrue(evt.connection == nil)
	assertTrue(evt.error ~= nil)
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="127.0.0.1", port="808080"} )
