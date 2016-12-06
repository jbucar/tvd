-- Test #drawPolygon in close mode
canvas:attrColor("red")
local polygonDrawer = canvas:drawPolygon("close")
polygonDrawer(0,100)(100,0)(100,100)()
canvas:flush()