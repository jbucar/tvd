function handler(evt)
	canvas:attrColor(0,0,0,0)
	canvas:clear()
	if evt.class == 'ncl' and evt.type == 'presentation' and evt.action == 'start' then
		event.post({class = 'ncl', type = 'presentation', action = 'stop', value=""})
	end
end
event.register(handler)
