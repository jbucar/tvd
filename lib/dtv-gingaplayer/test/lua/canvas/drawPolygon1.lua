-- Test #drawPolygon in open mode
canvas:attrColor("red")
local polygonDrawer = canvas:drawPolygon("open")
polygonDrawer(0,100)(100,0)(100,100)()
canvas:flush()