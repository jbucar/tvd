--	Try to connect to a non valid ip
require 'testSystem.testSystem'

function tcp_handler(evt)
	assertTrue(evt.class == "tcp")
	assertTrue(evt.type == "connect")
	assertTrue(evt.port == "13")
	assertTrue(evt.host == "300.0.0.0")
	assertTrue(evt.connection == nil)
	assertTrue(evt.error ~= nil)
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="300.0.0.0", port="13"} )
