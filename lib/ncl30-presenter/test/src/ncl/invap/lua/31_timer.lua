s = 0
canvas:attrFont('Tiresias', 56)

function draw()
	canvas:attrColor('navy')
	canvas:clear()
	canvas:attrColor('white')
	canvas:drawText(320, 250, s)
	canvas:flush()
	s = s + 1
	event.timer(1000, draw)
end

draw()	


