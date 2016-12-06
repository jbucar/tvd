
function clear_text(x, y, str, aSurface)
 local w,h = aSurface:measureText(str)
 local r,g,b,a = aSurface:getColor()
 aSurface:setColor(237,240,255,255)
 aSurface:fillRect(x-10, y, w+10, h)
 aSurface:setColor(r,g,b,a)
end

function table.copy(t)
  local copy = {}
  for k,v in pairs(t) do
    copy[k] = v
  end
  return copy
end

function charAt(str, pos)
 return string.char(string.byte(str, pos))
end

function centerString(x1, x2, str, aSurface)
 local str_len = aSurface:measureText(str)
 local x = x1 + (x2 - x1 - str_len)/2 
 return x
end
