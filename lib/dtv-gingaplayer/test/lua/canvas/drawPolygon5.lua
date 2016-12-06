-- Test #drawPolygon sending a nil parameter and then send the points.
require 'testSystem.testSystem'

canvas:attrColor("red")
local polygonDrawer = canvas:drawPolygon("fill")
assertThrow(function() polygonDrawer()(0,100)(100,0)(100,100) end)
canvas:flush()