--[[
/*******************************************************************************

  Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of Ginga implementation.

    This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    Ginga is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de Ginga.

    Este programa es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    Ginga se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
]]--

--Constants
local CELL_WIDTH = 69 -- img 66 + 3 por linea
local CELL_HEIGHT = 69
local SCREEN_W, SCREEN_H = canvas:attrSize()
local offset = {x = 76, y = 131} --board offset

--[[*****CELL TYPES*****]]--
local EMPTY_CELL = 0
local BOX_CELL = 1
local TARGET_CELL = 2 
local BOTH_CELL = 3 --Celda con caja y target
local SOLID_CELL = 4

--[[*****RESOURCES*****]]--
local player_img = canvas:new('images/cyclope_solo.jpg') 
local target_img = canvas:new('images/ubicacion.jpg')
local box_img = canvas:new('images/frasco.jpg')
local boxPlaced_img = canvas:new('images/frasco_ok.jpg')
local solid_img = canvas:new('images/piedras.jpg')
local levelComplete_img = canvas:new('images/nivel_completo.jpg')
local restartLevel_img = canvas:new('images/restart_level.png')
local superWinner_img = nil
function getSuperWinnerImg() 
	if superWinnerImg == nil then superWinnerImg = canvas:new('images/super_winner.jpg') end
	return superWinnerImg
end

--[[*****LEVELS*****]]--
local CANT_LEVELS = 7 
local current_level = 1
local levels ={}
levels[1] = { BOARD_WIDTH = 5, BOARD_HEIGHT = 5}
levels[2] = { BOARD_WIDTH = 6, BOARD_HEIGHT = 5}
levels[3] = { BOARD_WIDTH = 5, BOARD_HEIGHT = 5}
levels[4] = { BOARD_WIDTH = 6, BOARD_HEIGHT = 6}
levels[5] = { BOARD_WIDTH = 5, BOARD_HEIGHT = 5}
levels[6] = { BOARD_WIDTH = 6, BOARD_HEIGHT = 6}
levels[7] = { BOARD_WIDTH = 5, BOARD_HEIGHT = 5}
for l=1, CANT_LEVELS do
	levels[l].BOARD_W_PIXELS = levels[l].BOARD_WIDTH * CELL_WIDTH
	levels[l].BOARD_H_PIXELS = levels[l].BOARD_HEIGHT * CELL_WIDTH
	levels[l].targets_to_win = 3 --default targets to win the level
end

local boards = {} --array of boards, one board x level
local current_board = boards[current_level] 
player = {}
local player_moved = false
local moves = 0 --cant of moves
local targets --cantidad de targets pisados
local win_level = false
local dirty_cells = {} -- list of dirty cells to redraw


function reset_level(l)
	--Init the board with empty cells
	for i=0,levels[l].BOARD_WIDTH - 1 do
		for j=0,levels[l].BOARD_HEIGHT -1 do
			boards[l][i][j] = EMPTY_CELL
		end
	end

--[[*****LEVEL 1*****]]--
	if l == 1 then
		--Setea las posiciones de los targets
		boards[1][0][1] = TARGET_CELL
		boards[1][0][2] = TARGET_CELL
		boards[1][1][2] = TARGET_CELL
		--Setea las posiciones de las cajas
		boards[1][2][1] = BOX_CELL
		boards[1][2][2] = BOX_CELL
		boards[1][2][3] = BOX_CELL
		--Setea las posiciones de las bloques solidos
		boards[1][1][1] = SOLID_CELL
		boards[1][1][3] = SOLID_CELL
		--Posicion inicial del player
		levels[1].player_x = 3
		levels[1].player_y = 3
		levels[1].targets = 0 -- targets pisados inicialmente

--[[*****LEVEL 2*****]]--
	elseif l == 2 then
		boards[2][1][1] = TARGET_CELL
		boards[2][1][2] = TARGET_CELL
		boards[2][2][2] = TARGET_CELL
		--Setea las posiciones de las cajas
		boards[2][4][1] = BOX_CELL
		boards[2][3][2] = BOX_CELL
		boards[2][3][3] = BOX_CELL
		--Setea las posiciones de las bloques solidos
		boards[2][0][0] = SOLID_CELL
		boards[2][0][1] = SOLID_CELL
		boards[2][5][0] = SOLID_CELL
		boards[2][2][1] = SOLID_CELL
		boards[2][3][1] = SOLID_CELL
		boards[2][2][3] = SOLID_CELL
		--Posicion inicial del player
		levels[2].player_x = 5
		levels[2].player_y = 2
		levels[2].targets = 0 -- targets pisados inicialmente

--[[*****LEVEL 3*****]]--
	elseif l == 3 then
		boards[3][2][0] = TARGET_CELL
		boards[3][3][1] = BOTH_CELL
		boards[3][2][3] = BOTH_CELL
		--Setea las posiciones de las cajas
		boards[3][2][2] = BOX_CELL
		--Setea las posiciones de las bloques solidos
		boards[3][1][1] = SOLID_CELL
		boards[3][1][3] = SOLID_CELL
		--Columna de bloques solidos para no deschavar a sole
		boards[3][4][0] = SOLID_CELL
		boards[3][4][1] = SOLID_CELL
		boards[3][4][2] = SOLID_CELL
		boards[3][4][3] = SOLID_CELL
		boards[3][4][4] = SOLID_CELL
		--Posicion inicial del player
		levels[3].player_x = 0
		levels[3].player_y = 1
		levels[3].targets = 2 -- targets pisados inicialmente

--[[*****LEVEL 4*****]]--
	elseif l == 4 then
		boards[4][1][2] = TARGET_CELL
		boards[4][3][2] = TARGET_CELL
		boards[4][3][4] = BOTH_CELL
		--Setea las posiciones de las cajas
		boards[4][4][2] = BOX_CELL
		boards[4][2][3] = BOX_CELL
		--Setea las posiciones de las bloques solidos
		boards[4][0][0] = SOLID_CELL
		boards[4][0][1] = SOLID_CELL
		boards[4][1][0] = SOLID_CELL
		boards[4][4][0] = SOLID_CELL
		boards[4][5][0] = SOLID_CELL
		boards[4][5][1] = SOLID_CELL
		boards[4][5][2] = SOLID_CELL
		boards[4][0][4] = SOLID_CELL
		boards[4][0][5] = SOLID_CELL
		boards[4][1][4] = SOLID_CELL
		boards[4][1][5] = SOLID_CELL
		boards[4][2][4] = SOLID_CELL
		boards[4][2][5] = SOLID_CELL
		boards[4][4][4] = SOLID_CELL

		levels[4].targets = 1
		levels[4].player_x = 3
		levels[4].player_y = 3

--[[*****LEVEL 5*****]]--	
	elseif l == 5 then
		boards[5][1][0] = TARGET_CELL
		boards[5][3][0] = BOTH_CELL
		boards[5][2][2] = BOTH_CELL
		--Setea las posiciones de las cajas
		boards[5][1][2] = BOX_CELL
		--Setea las posiciones de las bloques solidos
		boards[5][0][3] = SOLID_CELL
		boards[5][0][4] = SOLID_CELL
		boards[5][4][3] = SOLID_CELL
		boards[5][4][4] = SOLID_CELL
		boards[5][2][1] = SOLID_CELL

		levels[5].targets = 2
		levels[5].player_x = 0
		levels[5].player_y = 2

--[[*****LEVEL 6*****]]--	
	elseif l == 6 then
		boards[6][2][3] = TARGET_CELL
		boards[6][4][2] = TARGET_CELL
		boards[6][5][2] = TARGET_CELL
		--Setea las posiciones de las cajas
		boards[6][1][2] = BOX_CELL
		boards[6][1][4] = BOX_CELL
		boards[6][4][1] = BOX_CELL
		--Setea las posiciones de las bloques solidos
		boards[6][0][2] = SOLID_CELL
		boards[6][0][3] = SOLID_CELL
		boards[6][0][4] = SOLID_CELL
		boards[6][0][5] = SOLID_CELL
		boards[6][3][5] = SOLID_CELL
		boards[6][4][5] = SOLID_CELL
		boards[6][5][5] = SOLID_CELL
		boards[6][2][2] = SOLID_CELL
		boards[6][3][2] = SOLID_CELL
		boards[6][3][3] = SOLID_CELL
		boards[6][3][0] = SOLID_CELL

		levels[6].targets = 0
		levels[6].player_x = 1
		levels[6].player_y = 1

--[[*****LEVEL 7*****]]--
	elseif l == 7 then
		boards[7][3][0] = TARGET_CELL
		boards[7][1][2] = TARGET_CELL
		boards[7][1][1] = BOTH_CELL
		--Setea las posiciones de las cajas
		boards[7][2][2] = BOX_CELL
		boards[7][3][3] = BOX_CELL
		--Setea las posiciones de las bloques solidos
		boards[7][0][0] = SOLID_CELL
		boards[7][0][4] = SOLID_CELL
		boards[7][4][3] = SOLID_CELL
		boards[7][4][4] = SOLID_CELL
		boards[7][2][3] = SOLID_CELL
		boards[7][3][1] = SOLID_CELL

		levels[7].targets = 1
		levels[7].player_x = 2
		levels[7].player_y = 4
	end
	reset_vars()
	draw()
end

function reset_vars()
	moves = 0
	win_level = false
	current_board = boards[current_level]
	targets = levels[current_level].targets 
	player.x = levels[current_level].player_x
	player.y = levels[current_level].player_y
end

function init_setup()
	--Init the boards with EMPTY cells
	for l=1,CANT_LEVELS do
		boards[l]={}
		for i=0,levels[l].BOARD_WIDTH - 1 do
			boards[l][i] = {}
		end
	end
	reset_level(current_level)
end

--[[*********Drawing Functions*************]]--
function drawBoardLines()
	canvas:attrColor(130, 130, 130, 255)
	--v lines
	for i=0, levels[current_level].BOARD_WIDTH do
		canvas:drawRect('fill', offset.x + i*CELL_WIDTH , offset.y, 3, levels[current_level].BOARD_H_PIXELS )
	end
	-- h lines
	for i=0, levels[current_level].BOARD_HEIGHT - 1 do
		canvas:drawRect('fill', offset.x, offset.y + i*CELL_HEIGHT, levels[current_level].BOARD_W_PIXELS, 3)
	end
	-- last h line
	canvas:drawRect('fill', offset.x, offset.y + levels[current_level].BOARD_HEIGHT*CELL_HEIGHT,levels[current_level].BOARD_W_PIXELS + 3, 3)
end

function drawBoard(board)
	--draw Board Background
	canvas:attrColor(10,10,10,255)
	canvas:drawRect('fill', offset.x, offset.y, levels[current_level].BOARD_W_PIXELS, levels[current_level].BOARD_H_PIXELS )
	--Draw board elements
	local x_pos, y_pos
	for i=0,levels[current_level].BOARD_WIDTH -1 do
		for j=0,levels[current_level].BOARD_HEIGHT - 1 do
			x_pos = offset.x + i*CELL_WIDTH + 3
			y_pos = offset.y + j*CELL_HEIGHT + 3 
			if board[i][j]  == BOX_CELL then
				canvas:compose(x_pos, y_pos , box_img)
			elseif board[i][j] == BOTH_CELL then
				canvas:compose(x_pos, y_pos , boxPlaced_img)
			elseif board[i][j]  == TARGET_CELL then
				canvas:compose(x_pos , y_pos, target_img)
			elseif board[i][j]  == SOLID_CELL then 
				canvas:compose(x_pos, y_pos, solid_img)
			end
		end
	end
	drawBoardLines()
end

function drawPlayer()
	local x_pos = offset.x + player.x  *CELL_WIDTH + 3
	local y_pos = offset.y + player.y * CELL_HEIGHT + 3
	canvas:compose(x_pos, y_pos, player_img)
	player_moved = false
--	print("BOXMAN --- Player drawed")
end

function drawInfo()
--	print("BOXMAN --- Info drawed")
	--Box
	canvas:attrColor(10,10,10,255)
	canvas:drawRect('fill', 508, offset.y, 165, 134)	
	canvas:attrColor(130, 130, 130, 255)
	canvas:drawRect('fill', 508, offset.y, 165, 3)
	canvas:drawRect('fill', 508, offset.y + 132, 165, 3)
	canvas:drawRect('fill', 508, offset.y , 3, 134)
	canvas:drawRect('fill', 673 , offset.y , 3, 135)

	--Box data
	canvas:attrColor('white')
	canvas:attrFont('Tiresias',18, nil)	
	canvas:drawText(523, offset.y + 17, "Nivel: " .. tostring(current_level))
	canvas:drawText(523, offset.y + 50, "Movimientos: ")
	canvas:drawText(640, offset.y + 50, tostring(moves))
	canvas:compose(508, offset.y + 140, restartLevel_img)
end

function updateInfo()
	local text = tostring(moves)
	local text_w, text_h = canvas:measureText(text)
	canvas:attrColor(0, 25, 10, 255)
	canvas:drawRect('fill', 640, offset.y + 50, text_w, text_h)
	canvas:attrColor('white')
	canvas:drawText(640, offset.y + 50, text)
end

function drawWinMsg()
	if current_level == 7 then --Si pasaste el ultimo nivel
		canvas:compose(0, 0, getSuperWinnerImg()) --Mostrar pantalla de super winner
	else
		canvas:compose(150, 120, levelComplete_img)
		canvas:attrColor('white')
		canvas:attrFont('Tiresias',20, nil)
		canvas:drawText(360, 210, "NIVEL " .. tostring(current_level))
	end
end

--[[Draw all]]--
function draw()
	canvas:attrColor(0,0,0,0)
	canvas:clear()
	drawBoard(boards[current_level])
	drawPlayer()
	drawInfo()
	canvas:flush()
	canvas:flush() -- flush extra para la copia en baja
end

--[[Repaint only the changes]]--
function update_screen()
	if (player_moved or win_level) then
		drawDirtyCells()
		if player_moved then drawPlayer() end
		drawInfo()
		if win_level then drawWinMsg() end
		canvas:flush()
		canvas:flush() -- flush extra para la copia en baja
	end
end

function drawDirtyCells()
	--print("BOXMAN --- " .. tostring(#dirty_cells) .. " Cell/s Drawed" )
	for i=1, #dirty_cells do
		x = dirty_cells[i].x
		y = dirty_cells[i].y
		x_pos = offset.x + (dirty_cells[i].x * CELL_WIDTH) + 3 
		y_pos = offset.y + (dirty_cells[i].y * CELL_HEIGHT) + 3
		if current_board[x][y]  == BOX_CELL then
			canvas:compose(x_pos , y_pos, box_img)
		elseif current_board[x][y] == BOTH_CELL then
			canvas:compose(x_pos, y_pos , boxPlaced_img)
		elseif current_board[x][y]  == TARGET_CELL then
			canvas:compose(x_pos , y_pos, target_img)
		else --empty cell
			canvas:attrColor(10,10,10,255)
			canvas:drawRect('fill', x_pos , y_pos , CELL_WIDTH - 3, CELL_HEIGHT  -3)
		end
	end
	dirty_cells = {}
end

--[[**************Move Related Functions*****************]]--
function hasBox(pos) 
	return current_board[pos.x][pos.y] == BOX_CELL or current_board[pos.x][pos.y] == BOTH_CELL
end
function isEmpty(pos)
	return current_board[pos.x][pos.y] == EMPTY_CELL
end
function isTarget(pos)
	return current_board[pos.x][pos.y] == TARGET_CELL
end
function hasTarget(pos)
	return current_board[pos.x][pos.y] == TARGET_CELL or current_board[pos.x][pos.y] == BOTH_CELL
end

function outBoard(dest)
	return dest.x < 0 or (dest.x > levels[current_level].BOARD_WIDTH - 1) or 
		   dest.y < 0 or dest.y > (levels[current_level].BOARD_HEIGHT- 1)
end

function move(cantX, cantY)
	player_moved = false
	box_moved = false
	local dest = { x = player.x + cantX , y = player.y + cantY}
	if not outBoard(dest) then
		if isEmpty(dest) or isTarget(dest) then --si el destino esta vacio
			dirty_cells[#dirty_cells + 1] = {x = player.x, y = player.y}				
			player.x = dest.x; player.y = dest.y --El player se mueve sin problemas	
			player_moved = true
		elseif hasBox(dest) then    --si hay una caja
			local box_dest = {x = dest.x + cantX, y = dest.y + cantY }
			if not outBoard(box_dest) and (isEmpty(box_dest) or isTarget(box_dest)) then --Si puede mover la caja
				dirty_cells[#dirty_cells + 1] = {x = player.x, y = player.y}								
				dirty_cells[#dirty_cells + 1] = box_dest
				box_moved = true 
				moves = moves + 1
				player.x = dest.x ; player.y = dest.y
				player_moved = true
				if isEmpty(box_dest) then
					current_board[box_dest.x][box_dest.y] = BOX_CELL
				else
					current_board[box_dest.x][box_dest.y] = BOTH_CELL  
					targets = targets + 1
				end			
				if hasTarget(dest) then --Si la caja estaba pisando un target
					current_board[dest.x][dest.y] = TARGET_CELL
					targets = targets -1 
				else
					current_board[dest.x][dest.y] = EMPTY_CELL	
				end
			end			
		end
	end
end

function change_level(level)
	if level > 0 and level <= CANT_LEVELS then		
		current_level = level
		reset_level(level)
	end
end

function getCellIncludingPosition(x,y)
	return (x - offset.x - 3) / CELL_WIDTH, (y - offset.y - 3) / CELL_HEIGHT
end

function onMouseButtonPressed(evt)
	-- We only handle mouse clicks in here.
	if evt.class == 'pointer' and evt.type == 'press' then
		-- if evt.x evt.y corresponden a celda adyacente
		-- generar un evento de teclado equivalente.
		if (win_level) then
			event.post({class='key', type='press', key='ENTER'})
			return true
		end
		evt.x, evt.y = tonumber(evt.x), tonumber(evt.y)
		if (evt.x >= 508 and evt.x <= 671 and
				evt.y >= offset.y + 140 and evt.y <= offset.y + 193) then
			event.post({class='key', type='press', key='ENTER'})
			return true
		end
		local clickX, clickY = getCellIncludingPosition(evt.x, evt.y)
		clickX = math.floor(clickX)
		clickY = math.floor(clickY)
		if (math.abs(player.x - clickX) + math.abs(player.y - clickY) ~= 1) then
			return true
		end
		if (player.x - clickX == -1) then
			event.post({class='key', type='press', key='CURSOR_RIGHT'})
		elseif(player.x - clickX == 1) then
			event.post({class='key', type='press', key='CURSOR_LEFT'})
		elseif(player.y - clickY == 1) then
			event.post({class='key', type='press', key='CURSOR_UP'})
		elseif(player.y - clickY == -1) then
			event.post({class='key', type='press', key='CURSOR_DOWN'})
		end	
		return true
	else
		return false
	end
end

function onKeyPress(evt)
	if evt.class == 'key' and evt.type == 'press' then
		if evt.key=='CURSOR_LEFT' then
			move(-1, 0)
		elseif evt.key=='CURSOR_RIGHT' then
			move(1, 0)
		elseif evt.key=='CURSOR_UP' then
			move(0, -1)
		elseif evt.key=='CURSOR_DOWN' then
			move(0, 1)
		elseif (evt.key=='ENTER' or evt.key =='OK') and win_level then
			if (current_level == 7) then change_level(1)
			else change_level(current_level + 1) end
		elseif (evt.key=='RED' or evt.key=="F1") then
			change_level(current_level - 1)
		elseif (evt.key =='GREEN' or evt.key=='F2') then
			change_level(current_level + 1)
		elseif evt.key=='ENTER' or evt.key=='OK' then
			reset_level(current_level)
		end

		if (targets == levels[current_level].targets_to_win) then
			win_level = true
		end
		update_screen()
	end
	return true 
end

init_setup()

-- We don't want to modify anything. We wish only to extend.
-- So, since the key listener handles EVERY event (i.e. It always returns true)
-- We add our listener before the one of the keyboard.
-- Besides, when a click is made on a valid cell, we generate an equivalent 
-- keyboard event. Soy everything is handled just as before.

event.register(onMouseButtonPressed)
event.register(onKeyPress)

