-- Test #drawPolygon with an invalid mode
require 'testSystem.testSystem'

canvas:attrColor("red")
assertThrow(function() canvas:drawPolygon("invalid") end)
canvas:flush()