-- Test draw two polygons
canvas:attrColor("red")

local polygonDrawer = canvas:drawPolygon("fill")
polygonDrawer(0, 100)(100, 0)(100, 100)

local polygonDrawer2 = canvas:drawPolygon("close")
polygonDrawer2(400, 500)(600, 300)(600, 500)

polygonDrawer()
polygonDrawer2()

canvas:flush()