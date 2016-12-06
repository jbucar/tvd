require '../testSystem.testSystem'

assertEquals( settings.channel.keyCapture, "" )

function my_handler( evt )
	if evt.class == "ncl" and evt.type == "attribution" then
		assertEquals( settings.channel.keyCapture, "4" )
	end
end

event.register(my_handler)
