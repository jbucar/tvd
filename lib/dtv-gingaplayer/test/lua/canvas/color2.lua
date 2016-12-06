-- Testea #attrColor cuándo se le pasan valores inválidos como parámetros
local x,y = 0,0
local predefinedInvalidedColours = { 'WHITE','AQUA', 'LIME', 'YELLOW', 'RED', 'FUCHSIA', 'PURPLE', 'MAROON',
'BLUE', 'NAVY', 'TEAL', 'GREEN', 'OLIVE', 'SILVER', 'GRAY', 'BLACK', 'invalid', 'INVALID'}
canvas:attrColor('white')
canvas:drawRect('fill',0,0,720,576)
canvas:flush()

for i,colour in ipairs(predefinedInvalidedColours) do
  canvas:attrColor(colour)
  canvas:drawRect("fill",x,y,50,50)
  x = x + 60
  if ((x + 60) >= 760) then
    x = 0
    y = y + 60
  end
end  
canvas:flush()