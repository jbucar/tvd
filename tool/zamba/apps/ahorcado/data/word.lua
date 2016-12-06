require 'data.utils'

class 'Word'

function initialize()
 self.category = nil
 self.word     = nil
 self.pullOfChars = nil
 self.dirty    = nil
 self.incompleteWord = nil
 self.goodChars = 0
end

function Word(category, word, aSurface)
 self.surface = aSurface
 self:reset(category,word)
end

function this:reset(category,word)
 self.category = category
 self.word = word
 self.pullOfChars = self.word
 self.incompleteWord = string.rep("_", string.len(self.word))
 self.goodChars = 0 --Amount of right chars selected
 self:updateChar(' ') --replace '_' with ' ' as appropiate
 self.drawCategory = true
end

function this:tryChar(aChar)
 if self:charInWord(aChar) then
  self:updateChar(aChar)
  return true
 else
  return false
 end
end

function this:updateChar(aChar)
 local find_index = 1
 local char_index = string.find(self.word, aChar, find_index)
 while char_index ~= nil do
  self.goodChars = self.goodChars + 1
  self.incompleteWord = string.sub(self.incompleteWord,1,char_index - 1) .. aChar .. string.sub(self.incompleteWord,char_index +1) 
  find_index = char_index + 1
  char_index = string.find(self.word, aChar, find_index)
 end
 self.dirty = true
end

function this:charInWord(aChar)
  if aChar ~= nil then
    local result = string.find(self.pullOfChars, aChar) ~= nil
    self.pullOfChars=string.gsub(self.pullOfChars,aChar,"")
    return result
  end
end

function this:draw()
 if self.dirty or self.drawCategory then
  self.surface:setFont('Tiresias', 25, nil)
  self.surface:setColor(0,0,0,255)
  local to_draw = ''
  local char = nil
  for i=1, #self.incompleteWord do
   char = charAt(self.incompleteWord, i)
   if char == ' ' then
    to_draw = to_draw .. string.rep(' ', 4)
   else
    to_draw = to_draw .. char .. ' '
   end
  end
  local x = centerString(75, 665, to_draw,self.surface) 

  local  MAX_X, MAX_Y = self.surface:getSize()
  local MIN_X = MAX_X * 0.05

  local y = MAX_Y - 150

  clear_text(x,y, to_draw,self.surface)
  self.surface:drawText(x, y+25, to_draw)
  self.dirty = false

  if self.drawCategory then
   self.surface:setFont('Tiresias', 20)
   self.surface:setColor(255,255,255,255)
   local x = centerString(185, 580, self.category,self.surface) 
   local y = 490+20
   self.surface:drawText(x, y, self.category)
   self.drawCategory = false
  end
  return true
 end
 return false
end

function this:wordComplete()
 return self.goodChars == self:size()
end

function this:size()
 return #self.word
end
