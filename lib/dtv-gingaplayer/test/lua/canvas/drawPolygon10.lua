-- Test #drawPolygon in a canvas smaller than its size
local newCanvas = canvas:new(50, 50)
newCanvas:attrColor("red")
local polygonDrawer = newCanvas:drawPolygon("fill")
polygonDrawer(0,100)(100,0)(100,100)()
canvas:compose(0, 0, newCanvas)
canvas:flush()