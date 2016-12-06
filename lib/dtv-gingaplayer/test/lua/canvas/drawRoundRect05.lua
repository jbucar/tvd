-- Testea #drawRoundRect pasándole como mode nil
local x,y = 10,20
local width,height = 60,60
local arcWidth, arcHeight = 10,10
canvas:attrColor("red")
canvas:drawRoundRect(nil,x,y,width,height,arcWidth, arcHeight)
canvas:flush()