-- Test #drawPolygon in a not main canvas
local newCanvas = canvas:new(150, 150)
newCanvas:attrColor("red")
local polygonDrawer = newCanvas:drawPolygon("fill")
polygonDrawer(0,100)(100,0)(100,100)()
canvas:compose(0, 0, newCanvas)
canvas:flush()