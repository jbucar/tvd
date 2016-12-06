require '../testSystem.testSystem'

assertEquals( settings.service.currentFocus, 0 )

function my_handler( evt )
	if evt.class == "ncl" and evt.type == "attribution" then
		assertEquals( settings.service.currentFocus, 1 )
	end
end

event.register(my_handler)
