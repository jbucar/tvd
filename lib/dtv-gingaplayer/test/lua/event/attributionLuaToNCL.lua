require 'testSystem.testSystem'

function sendEvent(evento)
	evento.action = 'start'
	event.post(evento)
	evento.action = 'stop'
	event.post(evento)
	assertTrue(true)
end
function handler(evt)
	local evento = {class = 'ncl',type = 'attribution',name = 'newBounds', value="hola"}
	sendEvent(evento)
end
event.register(handler)
