-- Testea que #drawText funcione para la fuente “Verdana” los estilos: “bold”, “italic” y “bold italic”
local texto = "Fuente Verdana "
canvas:attrFont("Verdana",20,nil)
canvas:attrColor("blue")
canvas:drawText(10,0,texto)
canvas:attrFont("Verdana",20,"bold")
canvas:attrColor("blue")
canvas:drawText(10,30,texto.."bold")
canvas:attrFont("Verdana",20,"italic")
canvas:drawText(10,60,texto.."italic")
canvas:attrFont("Verdana",20,"bold-italic")
canvas:drawText(10,90,texto.."bold-italic")
canvas:flush()