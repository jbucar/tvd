-- Testea que #drawText funcione con attrFont seteado para usar el estilo de tetra Teresias y Verdana ( ambas obligatorias )
local texto = "Fuente Tirerias de tamaño 20 px"

canvas:attrColor("green")

canvas:attrFont("Tiresias",20,nil)
canvas:drawText(10,0,texto)

texto = "Fuente Verdana de tamaño 20 px"
canvas:attrFont("Verdana",20,nil)
canvas:drawText(10,30,texto)

canvas:flush()