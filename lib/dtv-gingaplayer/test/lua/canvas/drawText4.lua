-- Testea que #drawText funcione para la fuente “Tiresias” los estilos: “bold”, “italic” y “bold italic”
local texto = "Fuente Tiresias "
canvas:attrFont("Tiresias",20,nil)
canvas:attrColor("blue")
canvas:drawText(10,0,texto)
canvas:attrFont("Tiresias",20,"bold")
canvas:attrColor("blue")
canvas:drawText(10,30,texto.."bold")
canvas:attrFont("Tiresias",20,"italic")
canvas:drawText(10,60,texto.."italic")
canvas:attrFont("Tiresias",20,"bold-italic")
canvas:drawText(10,90,texto.."bold-italic")
canvas:flush()