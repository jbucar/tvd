-- Test #drawPolygon with negative x coord.
canvas:attrColor("red")
local polygonDrawer = canvas:drawPolygon("fill")
polygonDrawer(-1,100)(100,0)(100,100)()
canvas:flush()