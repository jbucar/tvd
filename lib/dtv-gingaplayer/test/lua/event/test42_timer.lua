require 'testSystem.testSystem'


s = 0
canvas:attrFont('Tiresias', 56)

function draw()
	print( "[lua] draw: " .. s )
	canvas:attrColor('navy')
	canvas:clear()
	canvas:attrColor('white')
	canvas:drawText(320, 250, s)
	canvas:flush()
	s = s + 1
	if s < 10 then 
       		event.timer(10, draw)
    	else 
		event.post( { class = "user", action="signal" } )
		assertEquals(s,10)
	end
end

draw()
event.post( { class = "user", action="wait" } )
