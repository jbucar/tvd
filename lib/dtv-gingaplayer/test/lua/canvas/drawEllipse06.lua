-- Testea que #drawEllipse no se dibuje ya que se sale del canvas
local xc,yc = 360,260
local width,height = 320,320
local ang_start, ang_end = 0,360
canvas:attrColor("green")
canvas:drawEllipse("fill",xc,yc,width,height,ang_start,ang_end)
canvas:flush()
