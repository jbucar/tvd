-- Testea que no se dibuje un pixel que esté fuera de los límites del canvas
subCanvas = canvas:new(50,50)
for i=0, 100 do
  subCanvas:pixel(i,i,255,0,0,255)
end
canvas:compose(50,50,subCanvas)
canvas:flush()


