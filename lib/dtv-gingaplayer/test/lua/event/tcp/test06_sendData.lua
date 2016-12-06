--	Try to send and receive data
require 'testSystem.testSystem'
local id = -1
local cont = 0
local value = "test"

function tcp_handler(evt)
	if (evt.type == "connect") then
		assertTrue(evt.class == "tcp")
		assertTrue(evt.port == "7")
		assertTrue(evt.host == "127.0.0.1")
		assertTrue(evt.connection ~= nil)
		assertTrue(evt.error == nil)
		
		id = evt.connection;	
		event.post( 'in', {class="tcp", connection=id, type="data", value=value} )
	else
		assertTrue(id ~= -1)
		assertTrue(evt.class == "tcp")
		assertTrue(evt.type == "data")
		assertTrue(evt.connection == id)

		assertTrue(evt.value == value)

		if (cont < 10) then
			value = value .. cont
			cont = cont + 1
			event.post( 'in', {class="tcp", connection=id, type="data", value=value} )
		else
			event.post( {class = 'user', action = 'signal'} )
		end
	end
end

event.register(1, tcp_handler, "tcp")
event.post( 'in', {class="tcp", type="connect", host="127.0.0.1", port="7"} )
event.post( {class = 'user', action = 'wait'} )
