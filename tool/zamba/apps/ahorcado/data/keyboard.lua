class 'Keyboard'

function initialize()
 self:reset()
end

function Keyboard( surface )
 self.surface = surface
 self.letras = require 'data.Letras'
 self.letras:new()
 self.letras:cargarLetrasBase()
end

function this:reset()
 self.currentPos = { ['row'] = 0, ['column'] = 0 }
 self.toDraw = {}
 self.toClear = {}
 self.chars = {
   [0]  = { [0] = {char='A',status=0}, {char='B',status=0}, {char='C',status=0}, {char='D',status=0}, {char='E',status=0}, {char='F',status=0}, {char='G',status=0} }
 , [1]  = { [0] = {char='H',status=0}, {char='I',status=0}, {char='J',status=0}, {char='K',status=0}, {char='L',status=0}, {char='M',status=0}, {char='N',status=0} }
 , [2]  = { [0] = {char='Ñ',status=0}, {char='O',status=0}, {char='P',status=0}, {char='Q',status=0}, {char='R',status=0}, {char='S',status=0}, {char='T',status=0} }
 , [3]  = { [0] = {char='U',status=0}, {char='V',status=0}, {char='W',status=0}, {char='X',status=0}, {char='Y',status=0}, {char='Z',status=0}, {char=nil,status=0} }
 }
 
 self.cursorDirty = true
end

function this:selectChar()
 local r = self.currentPos['row']
 local c = self.currentPos['column']
--  self.chars[r][c].char = nil
 if self.chars[r][c].status == 0 then
   self.chars[r][c].status = self.chars[r][c].status +1
 end
 local charSelected = self.chars[r][c].char
 self.letras.setStatus(charSelected,self.chars[r][c].status)
 table.insert(self.toDraw, {['row'] = r , ['column'] = c})
 self:draw()
 return charSelected

end

function this:selectRow(row)
 local last_pos = {['row'] = self.currentPos['row'], ['column'] = self.currentPos['column']}
 self.currentPos.row = row
 self.currentPos.column = 0
 if self:canMoveRight() then
  table.insert(self.toClear, last_pos)
  self.cursorDirty = true
 else
  self.currentPos = last_pos
 end
end

function this:leftChar()
 local r,c    = self.currentPos['row'],self.currentPos['column']
 local last_pos = { row = r, column = c}
 table.insert(self.toClear, last_pos)
 self.cursorDirty = true

 local next_c
 if r == 3 and c == 0 then
   next_c = 5
 else
   next_c = (c - 1) % 7
 end
 self.currentPos['column'] = next_c
 self.dirty = true
end

function this:rightChar()
 local r,c    = self.currentPos['row'],self.currentPos['column']
 local last_pos = { row = r, column = c}
 table.insert(self.toClear, last_pos)
 self.cursorDirty = true

 local next_c
 if r == 3 and c == 5 then
   next_c = 0
 else
   next_c = (c + 1) % 7
 end
 self.currentPos['column'] = next_c
 self.dirty = true
end

function this:upChar()
 local r,c    = self.currentPos['row'],self.currentPos['column']
 local last_pos = { row = r, column = c}
 table.insert(self.toClear, last_pos)
 self.cursorDirty = true
 local next_r 
 if r == 0 and c == 6 then
   next_r = 2
 else
   next_r = (r - 1) % 4
 end
 self.currentPos['row'] = next_r
 self.dirty = true
end

function this:downChar()
 local r,c    = self.currentPos['row'],self.currentPos['column']
 local last_pos = { row = r, column = c}
 table.insert(self.toClear, last_pos)
 self.cursorDirty = true
 local next_r 
 if r == 2 and c == 6 then
   next_r = 0
 else
   next_r = (r + 1) % 4
 end
 self.currentPos['row'] = next_r
 
 self.dirty = true
end


function this:draw()
 for k,v in pairs(self.toClear) do
  self:clearCursor(self.toClear[k])
 end 
 self.toClear = {}
 for k,v in pairs(self.toDraw) do
  self:drawSelection(self.toDraw[k])
 end 
 self.toDraw = {}
 return self:drawCursor()
end


--[[ private functions ]]--
function this:drawSelection(position)
 local r,c = position['row'],position['column']
 local x = 390 + (c * 32)
 local y = 265 + (r * 35)
 
 local charSelected = self.chars[r][c].char
 self.letras.setStatus(charSelected,self.chars[r][c].status+10)
 self.surface:blit(x, y, self.letras.getPic(charSelected))

end

function this:clearCursor(position)
 local r,c = position['row'],position['column']
 local x = 390 + (c * 32)
 local y = 265 + (r * 35)

 local charSelected = self.chars[r][c].char
 self.letras.setStatus(charSelected,self.chars[r][c].status)
 self.surface:blit(x, y, self.letras.getPic(charSelected))

end

function this:drawCursor()
 if self.cursorDirty then
  local r,c = self.currentPos['row'],self.currentPos['column']
  local x = 390 + (c * 32)
  local y = 265 + (r * 35)

  local charSelected = self.chars[r][c].char
  self.letras.setStatus(charSelected,self.chars[r][c].status+10)
  self.surface:blit(x, y, self.letras.getPic(charSelected))

  self.cursorDirty = false
  return true
 end
 return false
end

function this:canMoveRight()
 local r = self.currentPos['row']
 local next_c = (self.currentPos['column'] + 1) % 7
 local moves = 0
 while (self.chars[r][next_c] == nil and moves <= 7) do
  moves = moves + 1
  next_c = (next_c + 1) % 7
 end
 return moves <= 7
end

function this:currentChar()
 return self.chars[self.currentPos['row']][self.currentPos['column']]
end
