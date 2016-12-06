--background
canvas:attrColor('navy')
canvas:clear()

--top left
canvas:attrColor('white')
canvas:drawRect('frame', 50, 50, 250, 100)
canvas:drawRect('fill', 50, 200, 250, 100)

-- top left
for i=0, 15 do
	canvas:attrColor(0, 255, 0, 255 - (i * 16) )
	local y = 50 + (i * 10)
	canvas:drawLine(525, y, 680, y)
end

-- right
image = canvas:new('../resources/images/lifia.jpg')
canvas:compose(325, 50, image)

local colors = {"white", "black", "silver", "gray", "red" , "maroon" , "fuchsia", "purple", "lime", "green", "yellow", "olive", "blue", "navy", "aqua", "teal"}
local fontWeight = {"normal", "bold", "italic"}
local fontFamily = "Tiresias"

for i=0, 7 do
	for j=0,2 do
		local ff = ''
		ff = fontFamily
		local fw = fontWeight[j + 1]
		local offset = i * 2 + j
		canvas:attrFont(ff, 14 + offset, fw)
		canvas:attrColor(colors[i * 2 + (j % 2) + 1])
		canvas:drawText(40 + 225 * j , 310 + 30 * i, ff .. ' ' .. fw)
	end
end

-- flush
canvas:flush()

