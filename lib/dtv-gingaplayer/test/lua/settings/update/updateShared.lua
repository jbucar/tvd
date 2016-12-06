require '../testSystem.testSystem'

assertEquals( settings.shared.myVar, "val" )

function my_handler( evt )
	if evt.class == "ncl" and evt.type == "attribution" then
		assertEquals( settings.shared.myVar, "anotherVal" )
	end
end

event.register(my_handler)