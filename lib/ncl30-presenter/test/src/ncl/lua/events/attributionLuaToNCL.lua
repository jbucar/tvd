function sendEvent(evento)
	evento.action = 'start'
	event.post(evento) 
	evento.action = 'stop'
	event.post(evento)	
end
function handler(evt)
	canvas:attrColor(0,0,0,0)
	canvas:clear()
	local evento = {class = 'ncl',type = 'attribution', name = 'newBounds', value = '100,100,100,100'}
	if evt.class == 'key' and evt.type == 'press' then
		if evt.key == 'RED' then
			evento.value = '0,0,360,288'
			sendEvent(evento)
		else
			evento.value = '0%,0%,100%,100%'
			sendEvent(evento)
		end
	end
end
event.register(handler)
