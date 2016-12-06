-- require 'LOS'

class 'GameState'

function initialize()
	self.game = nil
end

function GameState(game)
	self.game = game
end

function this:draw() end

function this:upPressed() end

function this:downPressed() end

function this:leftPressed() end

function this:rightPressed() end

function this:enterPressed() end

