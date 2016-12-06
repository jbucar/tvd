function handler(evt)
	if evt.class == 'key' then
		canvas:attrColor('white')
		canvas:clear()
		canvas:attrColor('black')
		canvas:attrFont('Tiresias',22,'normal')
		canvas:drawText(100,100,('TIPO DE ACCION: '..evt.type))
		canvas:drawText(100,140,('BOTON PRESIONADO: '..evt.key))
		canvas:flush()
	end
end
event.register(handler)
canvas:attrColor('white')
canvas:clear()
canvas:flush()

