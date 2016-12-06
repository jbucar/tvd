-- require 'LOS'
require 'data/game_state'
require 'data/utils'

class 'DialogState' extends 'GameState'

function initialize() 
  self.winImage = nil
  self.loseImage = nil
  self.drawing = true
end

function this:getWinImage()
  if self.winImage == nil then
    self.winImage = canvas.createSurfaceFromPath("imgs/Ahorcado_Ganaste.jpg")
  end
  return self.winImage
end

function this:getLoseImage()
  if self.loseImage == nil then
    self.loseImage = canvas.createSurfaceFromPath("imgs/Ahorcado_Perdiste.jpg")
  end
  return self.loseImage
end


function DialogState(game, winDialog)
 super(self, game)
 self.winDialog = winDialog
end

function this:draw()
 self.drawing = true
 if self.winDialog then
  self.game.surface:blit(150,135,self:getWinImage())   
  self.game.currentWord:draw()
 else
  self.game.hangman:draw()
  self.game.surface:blit(150,135,self:getLoseImage())
 end
  self.drawing = false 
 return true
end

function this:upPressed() 
end

function this:downPressed() 
end

function this:leftPressed() 
end

function this:rightPressed()
end

function this:redPressed() 
end

function this:greenPressed() 
end

function this:bluePressed() 
end

function this:yellowPressed() 
end

function this:enterPressed()
  if not self.drawing then
    if not self.winDialog then 
      self.game.points = 0
      self.game.pointsChanged = true
  end
    self.game:newLevel()
    self.game:mainLoop()
 end
end

