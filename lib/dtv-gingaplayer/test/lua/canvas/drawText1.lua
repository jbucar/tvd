-- Testea que #drawText dibuje con distintos tamaÃ±os de pixels
local texto = "Texto tamaño "
local inc = 8
local y = 0
canvas:attrColor("white")
for i=1, 10 do
  tam = i*inc
  y = y + tam
  canvas:attrFont("Tiresias",tam)
  canvas:drawText(10,y,texto..tostring(tam))
end

canvas:flush()