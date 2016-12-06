function my_handler( evt )
	if evt.class == "ncl" and evt.type == "presentation" then
		if settings.shared.myVar == "value" then
			canvas:attrColor("blue")
			canvas:drawRect("fill",0,0,720,576)
			canvas:flush()
		end
	end
	if evt.class == "ncl" and evt.type == "attribution" and evt.name == "attr" then
		if settings.shared.myVar == "val" then
			canvas:attrColor("red")
			canvas:drawRect("fill",0,0,720,576)
			canvas:flush()
		end
	end
end
event.register(my_handler)
