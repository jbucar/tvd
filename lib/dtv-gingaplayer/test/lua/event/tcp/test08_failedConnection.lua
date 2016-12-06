-- Try to post a tcp connection event with invalid or incomplete data
require 'testSystem.testSystem'

function tcp_handler(evt)
	assertFalse(true)
end

event.register(1, tcp_handler, "tcp")

-- without host
assertThrow( function() event.post( 'in', {class="tcp", type="connect", port="13"}) end )

-- without port
assertThrow( function() event.post( 'in', {class="tcp", type="connect", host="127.0.0.1"}) end )

-- without host and port
assertThrow( function() event.post( 'in', {class="tcp", type="connect" }) end )

-- without type
assertThrow( function() event.post( 'in', {class="tcp", host="127.0.0.1", port="13"}) end )

-- with invalid type
assertThrow( function() event.post( 'in', {class="tcp", type="invalid", port="13"}) end )