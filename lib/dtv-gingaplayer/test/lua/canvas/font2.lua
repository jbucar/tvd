-- Testea que el método #attrFont funcione cuándo se le pasa como parámetro “nil” de igual manera que cuándo se le pasa nil.
local texto = "Fuente Verdana "
canvas:attrFont("Verdana",20,'nil')
canvas:attrColor("blue")
canvas:drawText(10,0,texto)
canvas:flush()