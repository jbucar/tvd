function handler(evt)
	if evt.class == 'ncl' and evt.type == 'presentation' and evt.action == 'start' then
		canvas:compose(0,0,canvas:new("../../../../images/blue.jpg"))
		canvas:flush()
	end
end
event.register(handler)
