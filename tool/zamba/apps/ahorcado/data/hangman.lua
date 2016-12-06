require 'data/utils'

local resources = {}
local function getResource(frame)
 if not resources[frame] then
  local src = 'imgs/Ahorcado_Muneco-' .. frame .. '.jpg'
  resources[frame] = canvas.createSurfaceFromPath(src)
 end
 return resources[frame]
end


class 'Hangman'

function initialize()
end

function Hangman( surface )
 self.surface = surface
 self:reset()
end

function this:reset()
 self.attempts = 8
 self.dirty = true
end

function this:substractChance()
 if self.attempts > 0 then --Si se selecciona muy rápido, puede que el contador quede < 0 antes de q se dibuje
  self.attempts = self.attempts -1
  self.dirty = true
 end
end

function this:draw()
 if not self.dirty then
  return false
 else
  local x = 150
  local y = 155
  local img = getResource(self.attempts)
  self.surface:blit(x, y, img)
  self.dirty = false
  return true
 end
end

function this:getAttempts()
 return self.attempts
end

