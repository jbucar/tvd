-- Test draw empty polygon
require 'testSystem.testSystem'

canvas:attrColor("red")
local polygonDrawer = canvas:drawPolygon("fill")
assertThrow(function() polygonDrawer() end)
canvas:flush()