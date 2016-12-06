-- Testea que se obtenga bien el color de pixelese
canvas:attrColor("red")
canvas:clear(0,0,100,100)
canvas:attrColor("green")
canvas:clear(10,10,80,80)
canvas:attrColor("blue")
canvas:clear(20,20,60,60)
canvas:attrColor("yellow")
canvas:clear(30,30,40,40)
canvas:attrColor("transparent")
canvas:clear(40,40,20,20)

for i=0, 99 do
  for j=0, 99 do
    local r, g, b, a = canvas:pixel(i,j)
    canvas:pixel(i+ 110, j, r, g, b, a )
  end
end
canvas:flush()