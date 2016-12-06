-- Test #drawPolygon with no mode
require 'testSystem.testSystem'

canvas:attrColor("red")
assertThrow(function() canvas:drawPolygon() end)
canvas:flush()