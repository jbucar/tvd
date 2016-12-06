function my_handler( evt )
	if evt.class == "ncl" and evt.type == "presentation" then
		if settings.default.focusBorderColor == "red" then
			canvas:attrColor("blue")
			canvas:drawRect("fill",0,0,720,576)
			canvas:flush()
		end
	end
end

event.register(my_handler)
