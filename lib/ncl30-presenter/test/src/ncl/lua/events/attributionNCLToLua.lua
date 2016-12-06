function handler(evt)
	canvas:attrColor(0,0,0,0)
	canvas:clear()
	if evt.type == 'attribution' and evt.name == 'luaColor' then
		canvas:compose(0,0,canvas:new("../../../../images/"..evt.value..".jpg"))
		canvas:flush()
	end
end
event.register(handler)
