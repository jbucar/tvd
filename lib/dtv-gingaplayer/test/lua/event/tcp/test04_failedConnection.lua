--	Try to connect to an unreachable host
require 'testSystem.testSystem'

function tcp_handler(evt)
	assertTrue(evt.class == "tcp")
	assertTrue(evt.type == "connect")
	assertTrue(evt.port == "13")
	assertTrue(evt.host == "172.16.0.206")
	assertTrue(evt.connection == nil)
	assertTrue(evt.error ~= nil)
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="172.16.0.206", port="13"} )
