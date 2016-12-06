-- Muestra en total 255 cuadrados uno de cada color de 25x25 pixels.
-- Esto testea que se muestren correctamente al usar #attrColor antes de dibujar un cuadrado con #drawRect
local x,y = 0,0
for i=1,255 do
  if (i < 127) then
    canvas:attrColor(i,i,0,255)
  else
    canvas:attrColor(i,127,i,255)
  end
  canvas:drawRect("fill",x,y,25,25)
  x = x + 35
  if ((x + 35) >= 760) then
    x = 0
    y = y + 35
  end
end  
canvas:flush()