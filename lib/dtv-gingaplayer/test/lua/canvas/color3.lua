-- Muestra en total 18 cuadrados uno de cada color de 50x50 pixels.
-- Esto testea que todos los colores de NCL estén definidos y se muestren correctamente al usar #attrColor antes de dibujar un cuadrado con #drawRect
local x,y = 0,0
local predefinedColors = { 'transparent','white','aqua', 'lime', 'yellow', 'red', 'fuchsia', 'purple', 'maroon',
'blue', 'navy', 'teal', 'green', 'olive', 'silver','black' ,'gray', 'magenta' }
for i,colour in ipairs(predefinedColors) do
  canvas:attrColor(colour)
  canvas:drawRect("fill",x,y,50,50)
  x = x + 60
  if ((x + 60) >= 760) then
    x = 0
    y = y + 60
  end
end  
canvas:flush()