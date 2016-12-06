-- Muestra en total 255 cuadrados uno de cada color de 25x25 pixels.
-- Esto testea que se muestren correctamente al usar #attrColor antes de dibujar un cuadrado con #drawRect
local x,y = 0,0
for r=1,255 do
  canvas:attrColor(r,0,0,255)
  canvas:drawRect("fill",x,y,25,25)
  x = x + 35
  if ((x + 35) >= 760) then
    x = 0
    y = y + 35
  end
end  
canvas:flush()