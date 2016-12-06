local self= {
      chars={},
      charsPics={},
      }

function self.new()
 self.chars = {
   ['A']={status=0}, ['B']={status=0}, ['C']={status=0}, ['D']={status=0}, ['E']={status=0}, ['F']={status=0}, ['G']={status=0}
 , ['H']={status=0}, ['I']={status=0}, ['J']={status=0}, ['K']={status=0}, ['L']={status=0}, ['M']={status=0}, ['N']={status=0}
 , ['Ñ']={status=0}, ['O']={status=0}, ['P']={status=0}, ['Q']={status=0}, ['R']={status=0}, ['S']={status=0}, ['T']={status=0}
 , ['U']={status=0}, ['V']={status=0}, ['W']={status=0}, ['X']={status=0}, ['Y']={status=0}, ['Z']={status=0}, 
 }
end

function self.getStatus(aChar)
  return self.chars[aChar].status
end

function self.setStatus(aChar,status)
  self.chars[aChar].status = status 
end
function self.setStatusBase(aChar)
  self.chars[aChar].status = 0
  if aChar == 'Ñ' then
    self.charsPics[aChar].pic = canvas.createSurfaceFromPath('imgs/Letra_'..'ENIE'..'_'..self.chars['Ñ'].status..'.jpg')
  else
    self.charsPics[aChar].pic = canvas.createSurfaceFromPath('imgs/Letra_'..aChar..'_'..self.chars[aChar].status..'.jpg')
  end
  
end

function self.getPic(aChar)
  if self.charsPics[aChar] then
  if aChar == 'Ñ' then
    self.charsPics[aChar].pic = canvas.createSurfaceFromPath('imgs/Letra_'..'ENIE'..'_'..self.chars['Ñ'].status..'.jpg')
  else
    self.charsPics[aChar].pic = canvas.createSurfaceFromPath('imgs/Letra_'..aChar..'_'..self.chars[aChar].status..'.jpg')
  end
  end
  return self.charsPics[aChar].pic
end


function self.cargarLetrasBase()
  for aChar in pairs(self.chars) do
    self.charsPics[aChar]={pic=nil}
  if aChar == 'Ñ' then
    self.charsPics[aChar].pic = canvas.createSurfaceFromPath('imgs/Letra_'..'ENIE'..'_'..self.chars['Ñ'].status..'.jpg')
  else
    self.charsPics[aChar].pic = canvas.createSurfaceFromPath('imgs/Letra_'..aChar..'_'..self.chars[aChar].status..'.jpg')
  end
  end
end

return self