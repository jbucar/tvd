require 'data/playing_state'
require 'data/dialog_state'
require 'data/keyboard'
require 'data/hangman'
require 'data/word'

class 'Game'

local game
local loop_interval = 100
local background = canvas.createSurfaceFromPath('imgs/Ahorcado_fondo.jpg')
local letras = canvas.createSurfaceFromPath('imgs/Ahorcado_Letras_Status_00.jpg')
local timerID

function initialize()
 Win_Dialog_State =  DialogState:new (self, true)
 Lose_Dialog_State = DialogState:new (self, false)
 Playing_State = PlayingState:new(self)
end

-- Public: --
function Game( aSurface, aStorageID )
 self.surface = aSurface
 self.storageID = aStorageID
 self.points = 0
 self.maxScore = storage.loadInt(self.storageID,0)
 self:resetTables()
 self.time_elapsed = 0 - loop_interval
 self.pointsChanged = true
 self.timeChanged   = false
 self.state = nil 
 game = self
end

function this:start()
 self.keyboard = Keyboard:new(self.surface)
 self.hangman = Hangman:new(self.surface)
 self:newLevel()
 self:mainLoop()
end

function this:lose()
 return self.time == 0 or self.hangman:getAttempts() == 0
end

function this:tryChar(aChar)
 if self.currentWord:tryChar(aChar) then
  if self.points <= 9900 then
   self.points = self.points + 5
  else
   self:setMaxScore(self.points)
   self.points = 0
  end
  self.pointsChanged = true
 else
  self.hangman:substractChance()
 end
end

function this:upPressed() 
 self.state:upPressed()
end
function this:downPressed() 
 self.state:downPressed()
end
function this:leftPressed() 
 self.state:leftPressed()
end
function this:rightPressed()
 self.state:rightPressed()
end
function this:enterPressed() 
 self.state:enterPressed()
end
function this:redPressed() 
 self.state:redPressed()
end
function this:greenPressed() 
 self.state:greenPressed()
end
function this:bluePressed() 
 self.state:bluePressed()
end
function this:yellowPressed() 
 self.state:yellowPressed()
end

function this:timeUpdated()
 return self.timeChanged
end
function this:pointsUpdated()
 return self.pointsChanged
end
function this:pointsDrawed()
 self.pointsChanged = false
end
function this:timeDrawed()
 self.timeChanged = false
end

-- Private --
function this:draw() 
 if self.state:draw() then
  self.surface:flush()
 end
end

function staticMainLoop()
 game:mainLoop()
end

function this:mainLoop()
 if not self:lose() then
  self.time_elapsed = self.time_elapsed + loop_interval
  if self.time_elapsed == 1000 then
   self.time = self.time - 1
   self.time_elapsed = 0
   self.timeChanged = true
  end
  if self.currentWord:wordComplete() then
   self.points = self.points + self.time
   self.pointsChanged = true
   self:setMaxScore(self.points)
   self.state = Win_Dialog_State
  else
   timerID = timer.register(loop_interval)
  end
 else
  self:setMaxScore(self.points)
  self.state = Lose_Dialog_State
  time_elapsed = 0
  self.points = 0
 end
 
 self:draw()
 
end


function this:timeOut( tID )
  if tID == timerID then
    self:mainLoop()
  end
end


function this:newLevel()
 self:reset()
 self.state = Playing_State
 self.currentWord = self:getRandomWord()
 self.time = self.currentWord:size() * 7
 self.surface:blit(0,0,background)
 self.surface:blit(390,265,letras)
end

function this:getRandomWord()
  if #self.categories == 0 then
    self:reset()
    self:resetTables()
  end
  local category_index = math.random(1, #self.categories)
  local category = self.categories[category_index]
  local word = math.random(1, #self.allWords[category_index])
  word = table.remove(self.allWords[category_index], word)
  if #self.allWords[category_index] == 0 then 
    table.remove(self.allWords, category_index)
    table.remove(self.categories, category_index) 
  end

  if currentWord then
    return currentWord:reset(category, word)
  else
    return Word:new(category, word, self.surface)
  end
end

function this:reset()
 self.keyboard:reset()
 self.hangman:reset()
 self.surface:clear()
end


function this:resetTables()
 self.categories = {
  [1] = 'Arte',
  [2] = 'Cine',
  [3] = 'Música',
  [4] = 'Ciencia',
  [5] = 'Literatura',
 }

 self.allWords = {
   [1] = { 'PINTURA', 'PICASSO', 'LIENZO', "EL DAVID" } ,
   [2] = { 'TOM HANKS', 'THE MATRIX', 'TOY STORY', 'RICARDO DARIN', 'EL SECRETO DE SUS OJOS', 'MEMENTO', 'ET' } ,
   [3] = { 'GUNS AND ROSES', 'CERATI', 'LOS BEATLES', 'LENNON', 'GUITARRA', 'PANDERETA', 'BATERIA', 'NOTA', 'LA', 'DO' } ,
  [4] = { 'EINSTEIN', 'PITAGORAS', 'LIFIA',  } ,
  [5] = { 'FONTANARROSA', 'STEPHEN KING', 'HAMLET', 'OTELLO', 'POE', 'BORGES' } ,
 }
end

function this:setMaxScore(aPoints)

  if aPoints > self.maxScore then 
    self.maxScore = aPoints
    --persistir los datos
    storage.saveInt(self.storageID,self.maxScore)
  end

end
