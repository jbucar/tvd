-- Test #drawPolygon without draw
canvas:attrColor("red")
local polygonDrawer = canvas:drawPolygon("fill")
polygonDrawer(0,100)(100,0)(100,100)
canvas:flush()