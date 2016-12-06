require 'data/game_state'

class 'PlayingState' extends 'GameState'

function initialize() end

function PlayingState(aGame)
 super(self, aGame)
end

function this:draw() 
 local somethingDrawed = self.game.hangman:draw() or self.game.keyboard:draw() or self.game.currentWord:draw()
 return somethingDrawed or self:drawInfo()
end

function this:upPressed() 
 self.game.keyboard:upChar() 
end

function this:downPressed() 
 self.game.keyboard:downChar()
end

function this:leftPressed() 
 self.game.keyboard:leftChar()
end

function this:rightPressed() 
 self.game.keyboard:rightChar()
end

function this:enterPressed() 
 local char = self.game.keyboard:selectChar()
 self.game:tryChar(char)
end
function this:redPressed() 
 self.game.keyboard:selectRow(0)
end
function this:greenPressed() 
 self.game.keyboard:selectRow(1)
end
function this:bluePressed() 
 self.game.keyboard:selectRow(2)
end
function this:yellowPressed() 
 self.game.keyboard:selectRow(3)
end


-- private
function this:drawInfo()
 local drawed = false
 if self.game:timeUpdated() or self.game:pointsUpdated() then

  local  MAX_X, MAX_Y = self.game.surface:getSize()
  local x = MAX_X - 185
  local y = 173+25
  self.game.surface:setColor(255,255,255,255)
  self.game.surface:setFont('Tiresias', 30)
  if self.game:pointsUpdated() then
   local str = self.game.points
   self:clearCounterZone(x-125,y-30)
   local centerPosX  = centerString(x-125, x+203, str,self.game.surface)
   self.game.surface:drawText(centerPosX - 125, y, str)   
   self.game:pointsDrawed()
  end
  if self.game:timeUpdated() then
   local str = self.game.time  
    self:clearCounterZone(x-15,y-30)
   if str > 99 then
    self.game.surface:drawText(x-10, y,  str)
   else
    self.game.surface:drawText(x, y,  str)
   end
   self.game:timeDrawed()
  end
  drawed = true
 end
 return drawed
end

function this:clearCounterZone(x,y)
 local r,g,b,a = self.game.surface:getColor()
 self.game.surface:setColor(102,46,145,255)
 self.game.surface:fillRect(x, y, 78, 48)
 self.game.surface:setColor(r,g,b,a)
end