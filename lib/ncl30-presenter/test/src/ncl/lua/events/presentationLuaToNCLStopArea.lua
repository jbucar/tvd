function sendEvent(evento)
	evento.action = 'start'
	event.post(evento) 
	evento.action = 'stop'
	event.post(evento)
	
end
function handler(evt)
	local evento = {class = 'ncl',type = 'presentation', action = 'stop', label = 'a1'}
	sendEvent(evento)
end
event.register(handler)
