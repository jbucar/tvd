-- Testea que #attrOpacity retorne la opacidad del canvas cuándo no recibe parámetros
local image = canvas:new("resources/lifia.png")
image:attrOpacity(134)
if (image:attrOpacity() == 134) then
	canvas:compose(0,0,image)
end
canvas:flush()