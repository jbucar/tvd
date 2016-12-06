require '../testSystem.testSystem'

assertEquals( settings.default.focusBorderColor, "white" )
function my_handler( evt )
	if evt.class == "ncl" and evt.type == "attribution" then
		assertEquals( settings.default.focusBorderColor, "green" )
	end
end

event.register(my_handler)
