-- testea que #attrColor cuando no reciba parámetros retorne la composición RGBA del color del canvas ( un número por cada componente de 0 a 255) 
color = {r = nil, g = nil, b = nil, a = nil}
canvas:attrColor("red")
color.r, color.g, color.b, color.a = canvas:attrColor()
canvas:attrColor("green")
if ((color.r == 255) and (color.g == 0) and (color.b == 0) and (color.a == 255)) then
  canvas:attrColor(color.r,color.g, color.b, color.a)
end
canvas:drawRect('fill', 0, 0, 100,100)
canvas:flush()