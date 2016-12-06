----------------------------------------------------------------------------
-- Show schedule widget
----------------------------------------------------------------------------
-- NOTE:

-- This script models the behavior of the program guide grid.
-- From this grid the user can select a program and obtain related info.
-- The display of this information is responsibility of the EPG itself, not the schedule.

-- This file contains two classes: EPGScheduleClass and EPGScheduleBlockClass.
-- The former models the grid of blocks itself.
-- The latter models one of this blocks.


local EPG_SCHEDULE_CHANNEL_LIST_X = 75
local EPG_SCHEDULE_CHANNEL_LIST_Y = 356
local EPG_SCHEDULE_CHANNEL_LIST_H = 148
local EPG_SCHEDULE_CHANNEL_LIST_W = 110
local EPG_SCHEDULE_CHANNEL_LIST_GAP = 1

local EPG_SCHEDULE_DATE_CORNER_W = 116
local EPG_SCHEDULE_DATE_CORNER_H = 23

local EPG_SCHEDULE_GAP = 3
local METABLOCK_LABEL_COLOR = {r = 0, g = 0, b = 0, a = 255}
local BLOCK_LABEL_COLOR = {r = 255, g = 255, b = 255, a = 255}  
local BLOCK_LABEL_SELECTED_COLOR = {r = 0, g = 0, b = 0, a = 255}
local BLOCK_LABEL_FONT_SIZE = 12
local BLOCK_LABEL_BORDER = 5
local EPG_TIME_GAP = 1800

local EPG_NO_CHANNEL_MSG = " "
local EPG_TOOBIG_LBL_FILLER = "..."
local EPG_SCHEDULE_BLOCK_DEFAULT_COLOR = {r = 48, g = 48, b = 48, a = 255}
local EPG_SCHEDULE_BLOCK_SELECTED_COLOR = { r = 0, g = 183, b = 243, a = 255 }
local EPG_SCHEDULE_METABLOCKS_COLOR = {r = 50, g = 130 , b = 35 , a = 255 }
local EPG_SCHEDULE_TEXT_BLOCKS_NO_INFO = " "
local EPG_SCHEDULE_TEXT_BLOCKS_NO_CHANNEL = " "

local HOUR_BLOCKS_X_POSITION = 192
local HOUR_BLOCKS_WIDTH = 144

local EPG_SCHEDULE_BACKGROUND_COLOR = {r = 0, g = 0, b = 0, a = 255}
local EPG_SCHEDULE_BACKGROUND_RECT = {x = 190, y = 358, w = 429, h = 142}
local EPG_SCHEDULE_POSITION = {x = 193, y = 356}
local EPG_SCHEDULE_SCROLLBAR_RECT = {x = 626, y = 358, w = 16, h = 141}
local EPG_SCHEDULE_SCROLLBAR_VERTICAL_WASTE = 3
local EPG_SCHEDULE_SCROLLBAR_TOP_BORDER = "ScrollBarritaArriba.png"
local EPG_SCHEDULE_SCROLLBAR_BORDERS_HEIGHT = 7
local EPG_SCHEDULE_SCROLLBAR_BOTTOM_BORDER = "ScrollBarritaAbajo.png"
local EPG_SCHEDULE_SCROLLBAR_BODY = "ScrollBarritaMedio.png"


--------------------------------------------------------------------------
-- EPG Schedule Class                                                   --
--------------------------------------------------------------------------

EPGScheduleClass = {}   -- This class models the grid of blocks. It's composed of EPGScheduleBlock(Class)'s
EPGScheduleClass.__index = function (t,k) return EPGScheduleClass[k] end


function EPGScheduleClass:getIndexOfCurrentlySelectedChannel( )
	local index = self.channelManager.currentChannel()
	for k,v in pairs( self.channelManager.getAll() ) do
		if (v.channelID == index) then return k end
	end
	return -1
end


function EPGScheduleClass.new(x,y,w,h,bw,bh,hourBlocks,target,channelManager,clock, surfaceManager)
	local self = setmetatable({}, EPGScheduleClass)
	
	self.channelManager = channelManager
	self.clock = clock
	self.renderingManager = renderingManager

	self.x = x
	self.y = y
	self.w = w
	self.h = h
	self.channelDisplayOffset = 0  -- Represents vertical position of the display window
                                   -- in the context of the complete channel list.
                                   -- This means that if this variable contains "1",
                                   -- Then the first element of the grid (from top to bottom)
                                   -- corresponds to the second element of the entire list of channels.
	self.scheduleWidthInBlocks= bw 
	self.scheduleHeightInBlocks = bh --Size of the grid in blocks
	self.hourBlocks = hourBlocks
	self.targetWdt = target --Background surface over which this is rendered.
	self.selectedShow = nil --Show to display in EPG(Description, name, number, etc.) 
	self.timeOffset = 0 -- Horizontal offset in timeline (rep. in seconds from epoch)
	self:updateTime()
	self:createBlocks()
	self:resetBlockSelection()
	self.selectedChannel = 0

	self.scrollbarTop = surfaceManager.createSurfaceFromPath(EPG_SCHEDULE_SCROLLBAR_TOP_BORDER) 
	self.scrollbarBottom = surfaceManager.createSurfaceFromPath(EPG_SCHEDULE_SCROLLBAR_BOTTOM_BORDER)
	self.scrollbarBody = surfaceManager.createSurfaceFromPath(EPG_SCHEDULE_SCROLLBAR_BODY)

	return self
end 

function EPGScheduleClass:resetBlockSelection( )
	local currentChannel = self:getIndexOfCurrentlySelectedChannel()
	self.channelDisplayOffset = 0			   
	while ( self.channelDisplayOffset + self.scheduleHeightInBlocks < currentChannel )
		do self.channelDisplayOffset = self.channelDisplayOffset + 1 end
	self.rowOfSelectedBlock = 1
	if (currentChannel > 0) then
		self.rowOfSelectedBlock = currentChannel - self.channelDisplayOffset
	end
	self:updateTime()
	self.timeOffset = 0
	self.columnOfSelectedBlock = 1
end

function EPGScheduleClass:goOneBlockUp( )
	if (self.rowOfSelectedBlock > 1) then 
		self.rowOfSelectedBlock = self.rowOfSelectedBlock - 1 
	else
		self:upOneChannel()
	end
	self:update()
end

function EPGScheduleClass:goOneBlockDown()
	if (self.rowOfSelectedBlock < self.scheduleHeightInBlocks) then 
		self.rowOfSelectedBlock = self.rowOfSelectedBlock + 1 
	else
		self:downOneChannel()
	end
	self:update()
end

function EPGScheduleClass:goOneBlockRight( )
	-- Analizando si estoy en el borde
	local iAmBorderBlock = true
	for i=self.scheduleWidthInBlocks,self.columnOfSelectedBlock+1,-1 do
		if not self.blocks[self.rowOfSelectedBlock][i].isFused then
			iAmBorderBlock = false
		end
	end

	if (iAmBorderBlock) then
		self:forwardHalfAnHour()
		self:update()
		self.columnOfSelectedBlock = self.scheduleWidthInBlocks
	else
		-- No estoy en el borde.
		self.columnOfSelectedBlock = self.columnOfSelectedBlock + 1
		while self:getSelectedBlock().isFused do 
			self.columnOfSelectedBlock = self.columnOfSelectedBlock +1
		end
	end
	self:update()
end


function EPGScheduleClass:goOneBlockLeft( )
	if (self.columnOfSelectedBlock == 1) then
		self:backwardHalfAnHour()
	else
		-- No estoy en el borde.
		self.columnOfSelectedBlock = self.columnOfSelectedBlock - 1
	end
	self:update()
end

function EPGScheduleClass:tuneSelectedChannel( )
	for _,row in pairs(self.blocks) do
		for _,block in pairs(row) do
			block:tuneYourChannelIfSelected()
		end
	end
end

function EPGScheduleClass:updateTime( )
	local currentTime = self.clock.getTime()
	self.currentTableTime = currentTime - currentTime % EPG_TIME_GAP
end


function EPGScheduleClass:shouldBeAbleToGoDown( )
	return self.channelDisplayOffset + self.scheduleHeightInBlocks < self.channelManager.count()    
end

function EPGScheduleClass:onHide( )
	self.dateTopLeftCorner:onHide()
	for _,b in pairs(self.channelNameList) do b:onHide() end
	for _,b in pairs(self.hourDivisionRow) do b:onHide() end
	for _,row in pairs(self.blocks) do
		for _,block in pairs(row) do
			block:onHide()
		end
	end
end


-----------------------------------------
-- This code adds 2 since the minimum block
-- 'measures' currently 15'. However, the
-- grid moves by 30'. So, when the window goes
-- left or right, it's actually moving 2 blocks.
----------------------------------------- 
function EPGScheduleClass:forwardHalfAnHour( )
	self.timeOffset = self.timeOffset + 2
end

function EPGScheduleClass:backwardHalfAnHour( )
	self.timeOffset = self.timeOffset - 2
end

function EPGScheduleClass:upOneChannel( )
	if (self.channelDisplayOffset > 0) then
		self.channelDisplayOffset = self.channelDisplayOffset - 1
	end
end

function EPGScheduleClass:downOneChannel( )
	if (self:shouldBeAbleToGoDown()) then
		self.channelDisplayOffset = self.channelDisplayOffset + 1
	end
end

function EPGScheduleClass:getSelectedBlock()
	return self.blocks[self.rowOfSelectedBlock][self.columnOfSelectedBlock]
end

function EPGScheduleClass:createBlocks( )

	local bw,bh = self.scheduleWidthInBlocks, self.scheduleHeightInBlocks
	local blockWidth = (self.w - self.hourBlocks * EPG_SCHEDULE_GAP) / (self.hourBlocks +1)
	local blockHeight = (self.h - bh * EPG_SCHEDULE_GAP) / bh
	local target = self.targetWdt

	self.dateTopLeftCorner = EPGScheduleBlockClass.new(target, "NO CONTENT YET",
														self.x, self.y,
														EPG_SCHEDULE_DATE_CORNER_W,
														EPG_SCHEDULE_DATE_CORNER_H,
														bw, METABLOCK_LABEL_COLOR)
	self.dateTopLeftCorner.ownColor = false

	------------------------------------------------------
	-- CHANNEL LIST NAME LEFT PANNEL
	------------------------------------------------------

	local channelListBlockHeight = (EPG_SCHEDULE_CHANNEL_LIST_H - 
										(bh-1)*EPG_SCHEDULE_CHANNEL_LIST_GAP) / bh

	self.channelNameList = {}
	for i = 1,bh do
		self.channelNameList[i] = EPGScheduleBlockClass.new(target, "NO CONTENT YET",
													EPG_SCHEDULE_CHANNEL_LIST_X ,
													EPG_SCHEDULE_CHANNEL_LIST_Y + 
														(channelListBlockHeight + EPG_SCHEDULE_CHANNEL_LIST_GAP) * (i-1),
													EPG_SCHEDULE_CHANNEL_LIST_W,
													channelListBlockHeight, bw, METABLOCK_LABEL_COLOR)
		self.channelNameList[i].ownColor = false
	end

	-----------------------------------------
	-- ROW FOR HOUR DIVISION
	-----------------------------------------
	self.hourDivisionRow = {}

	for j = 1, self.hourBlocks do
		self.hourDivisionRow[j] = EPGScheduleBlockClass.new(
								target, "NO CONTENT YET",
								HOUR_BLOCKS_X_POSITION + HOUR_BLOCKS_WIDTH * (j-1),
								self.y ,
								blockWidth, blockHeight, self.hourBlocks - j, METABLOCK_LABEL_COLOR)
		self.hourDivisionRow[j].ownColor = false
	end



	local programsX = EPG_SCHEDULE_BACKGROUND_RECT.x + 4
	local programsY = EPG_SCHEDULE_BACKGROUND_RECT.y
	bw,bh = self.scheduleWidthInBlocks, self.scheduleHeightInBlocks
	blockWidth = (EPG_SCHEDULE_BACKGROUND_RECT.w - (bw - 1) * EPG_SCHEDULE_GAP) / (bw)
	blockHeight = (EPG_SCHEDULE_BACKGROUND_RECT.h - (bh - 1) * EPG_SCHEDULE_GAP) / (bh)

	self.blocks = {}
	for i = 1,bh do
		self.blocks[i] = {}
		for j = 1, bw do
			self.blocks[i][j] = EPGScheduleBlockClass.new(target, "NO CONTENT YET", 
													  programsX + (blockWidth + EPG_SCHEDULE_GAP) * (j-1),
													  programsY + (blockHeight + EPG_SCHEDULE_GAP) * (i-1),
													  blockWidth, blockHeight, bw - j, lblColor)
			self.blocks[i][j]:setChannelManager(self.channelManager)
		end
	end
	for i = 1,bh do
		self.blocks[i][1]:setAsFirstOfRow()
	end
end


function EPGScheduleClass:update( )
	self:fillBlocks()
	for _,row in pairs(self.blocks) do
		for _,block in pairs(row) do
			block:updateData()
			block:setSelected(false)
		end
	end
	self:getSelectedBlock():setSelected(true)
	while (self:getSelectedBlock().isFused) do
		self:getSelectedBlock():setSelected(false)
		self.columnOfSelectedBlock = self.columnOfSelectedBlock - 1
		self:getSelectedBlock():setSelected(true)
		self:getSelectedBlock():updateData()
	end
end


function EPGScheduleClass:draw( )
	local set_color = function (color) self.targetWdt._surface:setColor(color.r, color.g, color.b, color.a) end
	local fill_rect = function (rect)  self.targetWdt._surface:fillRect(rect.x, rect.y, rect.w, rect.h) end
	local blit = function (surface, x, y) self.targetWdt._surface:blit(x,y,surface) end

	set_color(EPG_SCHEDULE_BACKGROUND_COLOR)
	fill_rect(EPG_SCHEDULE_BACKGROUND_RECT)

	--------------------------------
	-- Scroll bar                 --
	--------------------------------

	set_color(EPG_SCHEDULE_BLOCK_DEFAULT_COLOR)
	fill_rect(EPG_SCHEDULE_SCROLLBAR_RECT)
	local surfaceAvailable = (EPG_SCHEDULE_SCROLLBAR_RECT.h - EPG_SCHEDULE_SCROLLBAR_VERTICAL_WASTE * 2)
	local barPosition = (self.channelDisplayOffset) * surfaceAvailable / math.max(self.channelManager.count(),1)
	barPosition = math.floor(barPosition+.5)	

	local barHeight = surfaceAvailable * math.min(1, (self.scheduleHeightInBlocks) / math.max(self.channelManager.count(),1)) 
	barHeight = math.floor(barHeight+.5)
	
	local barStart = EPG_SCHEDULE_SCROLLBAR_RECT.y + EPG_SCHEDULE_SCROLLBAR_VERTICAL_WASTE + barPosition
	local borderH = EPG_SCHEDULE_SCROLLBAR_BORDERS_HEIGHT
	blit( self.scrollbarTop, EPG_SCHEDULE_SCROLLBAR_RECT.x + 2, barStart )
	for y=barStart + borderH,barStart + barHeight - borderH, 1 do
		blit( self.scrollbarBody, EPG_SCHEDULE_SCROLLBAR_RECT.x + 2, y )	
	end
	blit( self.scrollbarBottom, EPG_SCHEDULE_SCROLLBAR_RECT.x + 2, barStart + barHeight - borderH )
	
	self.dateTopLeftCorner:draw()
	for _,b in pairs(self.channelNameList) do b:draw() end
	for _,b in pairs(self.hourDivisionRow) do b:draw() end

	--------------------------------
	-- Blocks                     --
	--------------------------------
	for _,row in pairs(self.blocks) do
		for _,block in pairs(row) do
			block:draw()
		end
	end
end


function EPGScheduleClass:refresh( )
	self:update()
	self:draw()	
end

function EPGScheduleClass:fillBlocks( )
	self:updateTime()

	------------------------------------------------
	-- HOUR DIVISION ROW
	------------------------------------------------

	local bw,bh = self.scheduleWidthInBlocks, self.scheduleHeightInBlocks

	for i = 1, self.hourBlocks do
		self.hourDivisionRow[i]:setText(os.date("%H:%M", self.currentTableTime + (self.timeOffset + (i - 1) * 2 ) * EPG_BLOCK_TIME_GAP))
	end

	------------------------------------------------
	-- BLOCKS PROPIAMENTE DICHOS
	------------------------------------------------
	for i = 1, bh do
		for j = 1,bw do
			self.blocks[i][j]:setTime(self.currentTableTime + (self.timeOffset + j - 1) * EPG_BLOCK_TIME_GAP)
			self.blocks[i][j]:setChannel(self.channelDisplayOffset+i)
		end
	end
	-------------------------------------------
	-- REEMPLAZAR CON CHANNEL NAME LIST
	-------------------------------------------
	local channelsList = self.channelManager.getAll()
	for i = 1, bh do
		local name = EPG_NO_CHANNEL_MSG 
		if (channelsList[i + self.channelDisplayOffset] ~= nil) then
			name = channelsList[i + self.channelDisplayOffset].name
		end
		self.channelNameList[i]:setText(name) 
	end
	------------------------------------------
	-- REEMPLAZAR CON CORNER DATE BLOCK
	------------------------------------------

	local date = self.currentTableTime + self.timeOffset * EPG_BLOCK_TIME_GAP
	local dateText = firstToUpper(os.date("%a", date)) ..
								  os.date(", %d de ", date) ..
					 firstToUpper(os.date( "%b", date))
	self.dateTopLeftCorner:setText(dateText) 
end



-------------------------------------------------------------------------------------
-- EPG Schedule Block class                                                        --
-------------------------------------------------------------------------------------
EPGScheduleBlockClass = {} -- El bloque es una entrada en la tabla, se asegura de no superar alto y ancho asignado.
EPGScheduleBlockClass.__index = function (t,k) return EPGScheduleBlockClass[k] end

function EPGScheduleBlockClass.new( targetWdt, aText, x, y, width, height, blocksLeftInRow, lblColor)
	local self = setmetatable({}, EPGScheduleBlockClass)
	self.x = x
	self.y = y
	self.w = width
	self.h = height
	self.minWidth = width
	self.minHeight = height
	self.targetWdt = targetWdt
	self.ownColor = true
	self.myShow = {}
	self.lblDefaultColor = lblColor or BLOCK_LABEL_COLOR
	local labelx, labely = self.x + BLOCK_LABEL_BORDER, self.y + self.h / 2 + BLOCK_LABEL_FONT_SIZE / 5
	self.label = lblNew( "A block label", labelx, labely, " ", BLOCK_LABEL_FONT_SIZE,  lblColor or BLOCK_LABEL_COLOR)
	self:setText(aText)
	self.assignedChannelId = -1
	self.assignedInstant = 0 -- Seconds from epoch
	self.imFirstOfRow = false -- True if this block is the first of the row. This means it can't be fused.
	self.blocksLeftInRow = blocksLeftInRow
	self.selected = false
	self.isFused = false -- True if this block has been absorved by another one.
	                     -- Which means that it has no graphical representation.
	return self
end

function EPGScheduleBlockClass:setChannelManager( aChannelManager )
	self.channelManager = aChannelManager
end

function EPGScheduleBlockClass:draw()
	local set_color = function (color) self.targetWdt._surface:setColor(color.r, color.g, color.b, color.a) end
	local fill_rect = function (rect)  self.targetWdt._surface:fillRect(rect.x, rect.y, rect.w, rect.h) end
	
	local newLabelColor = self.lblDefaultColor
	if (self.selected) then
		newLabelColor = BLOCK_LABEL_SELECTED_COLOR
		setShowSelectedBySchedule( self.myShow )
		setEPGSelectedChannel(self.channelManager.getAll()[self.assignedChannelId])
	end
	self.label.surface:setColor(newLabelColor.r,newLabelColor.g,newLabelColor.b,newLabelColor.a)

	if self.selected then
		set_color(EPG_SCHEDULE_BLOCK_SELECTED_COLOR)
	else
		if (not self.ownColor) then
			self:tryAndFitTextInLabel()
			lblOnShow(self.label)
			return 
		else
			set_color(EPG_SCHEDULE_BLOCK_DEFAULT_COLOR)
		end
	end
	fill_rect({x = self.x - self.targetWdt.x,
			   y = self.y - self.targetWdt.y,
			   w = self.w - 1,
			   h = self.h - 1})
	lblOnShow(self.label)
	self:tryAndFitTextInLabel()
end

function EPGScheduleBlockClass:onHide()
	lblOnHide(self.label)
end

function EPGScheduleBlockClass:setAsFirstOfRow()
	self.imFirstOfRow = true 
end

function EPGScheduleBlockClass:tryAndFitTextInLabel()
	local fittingWidth = string.len(self.text)
	local w,h,a = measureText(self.text, BLOCK_LABEL_FONT_SIZE)
	if ( w <= self.w - BLOCK_LABEL_BORDER * 2) then
		lblText(self.label, self.text)
		return
	end
	while (fittingWidth > 0) do
		fittingWidth = fittingWidth - 1
		local textToFit = string.sub( self.text, 1, fittingWidth ) .. EPG_TOOBIG_LBL_FILLER 
		local w,h,a = measureText(textToFit, BLOCK_LABEL_FONT_SIZE)
		if (w <= self.w - BLOCK_LABEL_BORDER * 2) then
			lblText(self.label,textToFit)
			return
		end
	end
	lblText(self.label, " ")
end

function EPGScheduleBlockClass:setSelected(value)
	self.selected = value
end

function EPGScheduleBlockClass:getShowForInstant( anInstant, allChannels)
	local myShows = self.channelManager.getShowsBetween( allChannels[self.assignedChannelId].channelID,
								 os.date("%Y-%m-%d %H:%M:%S", anInstant - EPG_BLOCK_TIME_GAP / 2 + 1),
								 os.date("%Y-%m-%d %H:%M:%S", anInstant + EPG_BLOCK_TIME_GAP / 2)
								)
	return getShowWithClosestStartToGiven( anInstant, myShows )
end

function EPGScheduleBlockClass:updateData()
    -- For blocks with no channel assigned
	self.myShowStart = 0
	local allChannels = self.channelManager.getAll()

	-- Description:
	-- 1. Get program and its duration
	-- 2. We check at which block of the program we stand. If we are not its first, we ignore.
	-- 3. If I'm at first block (of program or grid. It's the same),
	--           we calculate how many blocks is the program made of.
	-- 4. We set the text of the block with the name of the program and the width of the last step.

	if (allChannels[self.assignedChannelId] ~= nil) then
		local myShow = self:getShowForInstant(self.assignedInstant, allChannels)
		if ( myShow ~= nil) then
			self.myShow = myShow
			local secondsOfProgram = parseTimestampFromString(myShow.stopDate.day .. " " 
                                                           .. myShow.stopDate.time)
								   - parseTimestampFromString(myShow.startDate.day .. " " 
                                                           .. myShow.startDate.time) - 1  
			local startInSeconds = parseTimestampFromString( myShow.startDate.day .. " " 
														  .. myShow.startDate.time )
			self.myShowStart = startInSeconds
			if (   (self.assignedInstant <= startInSeconds + EPG_BLOCK_TIME_GAP / 2 and
					self.assignedInstant > startInSeconds - EPG_BLOCK_TIME_GAP / 2)
				or self.imFirstOfRow) then
				local blocksLeft = 0
				local blockIter = self.assignedInstant
				while (blockIter < startInSeconds + secondsOfProgram - EPG_BLOCK_TIME_GAP / 2
					   and blocksLeft < self.blocksLeftInRow + 1) do
					blockIter = blockIter + EPG_BLOCK_TIME_GAP
					blocksLeft = blocksLeft + 1
				end
				local blocksToFuse = blocksLeft 
				self.w = self.minWidth * blocksToFuse + (EPG_SCHEDULE_GAP * (blocksToFuse - 1))
				self.isFused = false
				self:setText(tostring(myShow.name))
			else
				self.w = 1
				self.isFused = true
				self:setText(" ")
			end
		else
			self:setText(EPG_SCHEDULE_TEXT_BLOCKS_NO_CHANNEL)
			self.myShow = {}
			if ( self.imFirstOfRow or 
				self:getShowForInstant(self.assignedInstant - EPG_BLOCK_TIME_GAP, allChannels) ~= nil) then
				local blocksToFuse = 1
				while ( self:getShowForInstant(self.assignedInstant + (blocksToFuse ) * EPG_BLOCK_TIME_GAP,
						allChannels) == nil and blocksToFuse <= self.blocksLeftInRow) do
					blocksToFuse = blocksToFuse + 1
				end
				self.w = self.minWidth * blocksToFuse + (EPG_SCHEDULE_GAP * (blocksToFuse - 1))
				self.isFused = false
			else
				self.w = 1
				self.isFused = true
			end
		end
	else
		-- At this point, this block has no channel assigned.
		self:setText(EPG_SCHEDULE_TEXT_BLOCKS_NO_CHANNEL)
		self.myShow = {}
		if ( self.imFirstOfRow ) then
		 	local blocksToFuse = self.blocksLeftInRow + 1
		 	self.w = self.minWidth * blocksToFuse + (EPG_SCHEDULE_GAP * (blocksToFuse - 1))
		 	self.isFused = false
		else
		 	self.w = 1
		 	self.isFused = true
		end
	end
	if (self.selected) then self:setSelected(true) end
end


function EPGScheduleBlockClass:setChannel(channelId)
	self.assignedChannelId = channelId
end

function EPGScheduleBlockClass:setTime(anInstant)
	self.assignedInstant = anInstant
end

function EPGScheduleBlockClass:tuneYourChannelIfSelected()
	local allChannels = self.channelManager.getAll()
	if (self.selected and self.assignedChannelId > 0 and self.assignedChannelId <= #(allChannels)) then
		changeChannel(allChannels[self.assignedChannelId].channelID)
		refreshEPGBackground(EPGWindow) 
	end
end

function EPGScheduleBlockClass:setText(aNewText)
	if (aNewText == "") then aNewText = " " end -- Canvas throws an exception if the string to render is empty.
	self.text = aNewText                        -- However, this strings sometimes appear as name of services.
end
