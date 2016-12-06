-- MAIN STAGE
-- Module MainStage
MainStage = {
}

local arrows
local nav_bar	 -- NavigationBar
local categories -- category Selection list
local contents -- content list
local page_counter
local background
local stage_surface
local stage_showing
local contents_dirty
local view_cache
local view_in_cache
local chapterRequests

setmetatable(MainStage,Object)
MainStage.super   = Object
MainStage.__index = MainStage  -- se prepara para ser Heredada
_G.MainStage      = MainStage

function MainStage:Init( mainSurface )
	stage_surface = mainSurface
	self.keysMap = KeysMap_getMapForStage( self )
end

-- LOAD FUNCTION
function MainStage:load( )
	chapterRequests = {}
	arrows = {}
	arrows.up = { x = 352, y = 100, pic = '', enable = false }
	arrows.down =	{ x = 352, y = 476, pic = '', enable = false }
	arrows.empty = { x = 352, y = 476, pic = '' }
	-- contentList
	contents = {
		x = 0,
		y = 0,
		default_bg = { x = 123, y = 116, pic = '' },
		total_items = 1,
		items_per_page = 5,
		items_in_page = 5,
		total_pages = 1,
		index = 1,
		current_page = 1,
		last_page_visited = 1,
		request_new_page = true,
		items_type = '',
		items_id = { '0', '0', '0', '0', '0' },
		items = {},
		data = '',
		font_size = 15,
	}
	contents.default_bg.pic = canvas.createSurfaceFromPath( 'resources/images/contentListBg.png' )
	contents.items[1] = { x = 123, y = 116, title = { x = 238, y = 136 }, chapters = { x = 243, y = 161 }, bgTxt = { x = 230, y = 116, w = 365, h = 61 }, pic = '' }
	contents.items[2] = { x = 123, y = 188, title = { x = 238, y = 208 }, chapters = { x = 243, y = 233 }, bgTxt = { x = 230, y = 188, w = 365, h = 61 }, pic = '' }
	contents.items[3] = { x = 123, y = 260, title = { x = 238, y = 280 }, chapters = { x = 243, y = 305 }, bgTxt = { x = 230, y = 260, w = 365, h = 61 }, pic = '' }
	contents.items[4] = { x = 123, y = 332, title = { x = 238, y = 352 }, chapters = { x = 243, y = 377 }, bgTxt = { x = 230, y = 332, w = 365, h = 61 }, pic = '' }
	contents.items[5] = { x = 123, y = 404, title = { x = 238, y = 424 }, chapters = { x = 243, y = 449 }, bgTxt = { x = 230, y = 404, w = 365, h = 61 }, pic = '' }
	-- Number of page
	page_counter = {}
	page_counter.txt = { x = 512, y = 485, font_size = 13 }
	page_counter.bg = { x = 512,	y = 472, pic = '' }
	stage_showing = false
	contents_dirty = false
	view_in_cache = false
	view_cache = canvas.createSurface( 0, 0, stage_surface:getSize() )
	arrows.up.pic = canvas.createSurfaceFromPath( 'resources/images/flechasArriba.png' )
	arrows.down.pic = canvas.createSurfaceFromPath( 'resources/images/flechasAbajo.png' )
	arrows.empty.pic = canvas.createSurfaceFromPath( 'resources/images/emptyArrow.png' )
	page_counter.bg.pic = canvas.createSurfaceFromPath( 'resources/images/pageCounterViewBg.png' )
	background	= canvas.createSurfaceFromPath( 'resources/images/background.png' )
	local help2 = canvas.createSurfaceFromPath( 'resources/images/Ayuda2.png' )
	background:blit( 0, 517, help2 )
	help2:destroy()
	-- LOADING NAVIGATION BAR
	local items = {}
	items[1] = { x = 124, y = 63, w = 125, h = 3 }
	items[2] = { x = 266, y = 63, w = 104, h = 3 }
	items[3] = { x = 388, y = 63, w = 50, h = 3 }
	items[4] = { x = 455, y = 63, w = 51, h = 3 }
	items[5] = { x = 524, y = 63, w = 136, h = 3 }
	nav_bar = NavigationBar:new( items )
	categories = {} -- category Selection list
	categories[1] = { id = '6', type = 'serie', name = 'SERIES-UNITARIOS' }
	categories[2] = { id = '8', type = 'serie', name = 'DOCUMENTALES' }
	categories[3] = { id = '7', type = 'clip', name = 'CORTOS' }
	categories[4] = { id = '17', type = 'serie', name = 'MICROS' }
	categories[5] = { id = '19', type = 'clip', name = 'IGUALDAD CULTURAL' }
	-- LOADING CONTENT LIST
	contents.x     = 123
	contents.y     = 116
	for i=1,contents.items_per_page do
		contents.items[ i ].pic = RemoteImage:new( contents.items[ i ].x, contents.items[ i ].y, 'resources/images/itemDefaultBg.png' )
	end
	for i=1,contents.items_per_page do
		contents.items[ i ].pic:setContainer( self )
		contents.items[ i ].pic:load()
	end
	contents.items[ contents.index ].pic:selected( true )
end

local function updateArrows()
	if ( contents.current_page ~= contents.last_page_visited ) then
		local upOldStatus = arrows.up.enable
		local downOldStatus = arrows.down.enable
		local upX, upY, upPic = arrows.up.x, arrows.up.y, arrows.up.pic
		local downX, downY, downPic = arrows.down.x, arrows.down.y, arrows.down.pic
		local emptyPic = arrows.empty.pic
		if ( contents.current_page == 1 ) then -- first page
			arrows.up.enable = false
			arrows.down.enable = true
		else
			if ( contents.current_page == contents.total_pages ) then -- last page
				arrows.up.enable = true
				arrows.down.enable = false
			else
				arrows.up.enable = true
				arrows.down.enable = true
			end
		end
		local upNewStatus = arrows.up.enable
		local downNewStatus = arrows.down.enable
		if ( upOldStatus ~= upNewStatus ) then
			if upNewStatus then
				stage_surface:blit( upX, upY, upPic ) -- enable
			else
				stage_surface:blit( upX, upY, emptyPic ) -- disable
			end
		end
		if ( downOldStatus ~= downNewStatus ) then
			if downNewStatus then
				stage_surface:blit( downX, downY, downPic ) -- enable
			else
				stage_surface:blit( downX, downY, emptyPic ) -- disable
			end
		end
	end
end

local function contentListRequestNewPage( itemType, category_id )
	contents.items_type = itemType
	chapterRequests = {} --clear data
	client:cancelHttpRequests()
	client:requestList( contentListProcessData, itemType, category_id, contents.items_per_page, contents.current_page )
	contents.request_new_page = false
end

function contentListProcessData( id, status, data )
	local all_response = jsonParser:decode( data )
	local responseType = contents.items_type..'s' -- el nombre de la key está en plural
	if ( status == 1 ) and stage_showing and all_response and all_response[ responseType ] then -- request ok
		local subfixIndex = 1
		contents.total_items = all_response.pagination.totalItems or 0 -- update total_items a un valor o a cero por defecto
		contents.total_pages = math.ceil( contents.total_items / contents.items_per_page )
		contents.data = all_response[ responseType ]
		contents.items_in_page = #contents.data -- cantidad items disponibles en una página
		stage_surface:setFont( "Tiresisas", contents.font_size )
		stage_surface:setColor( 255, 255, 255, 255 )
		local items = contents.items
		local itemsID = contents.items_id
		local upper = string.upper
		local resize = Tools.resize
		for i=1,#contents.data do
			local each = contents.data[i]
			local img = each.image
			items[ i ].pic:fetch( img.source..img.id..img.sizes[ subfixIndex ].subfix..'.'..img.type ) -- actualizo la imagen con la de la url
			itemsID[ i ] = each.id
			stage_surface:setFont( "Tiresisas", 16 )
			stage_surface:drawText( items[ i ].title.x, items[ i ].title.y, upper( resize( each.title ) ) )
			if each.chapters and each.chapters ~= '' then
				stage_surface:setFont( "Tiresisas", 14 )
				stage_surface:drawText( items[ i ].chapters.x, items[ i ].chapters.y, 'Capitulos : '..each.chapters )
			else
				chapterRequests[each.id] = { surface = stage_surface, x = items[ i ].chapters.x, y = items[ i ].chapters.y, size = 14, callback = fillChapter }
				client:requestItem( chapterRequests[each.id].callback, categories[ nav_bar:getIndex() ].type, each.id, categories[ nav_bar:getIndex() ].id )
			end
		end
		stage_surface:blit( page_counter.bg.x, page_counter.bg.y, page_counter.bg.pic ) -- clean bg
		if contents.total_pages > 0 then
			stage_surface:setFont( "Tiresisas", page_counter.txt.font_size )
			stage_surface:drawText( page_counter.txt.x, page_counter.txt.y, "Pág. "..contents.current_page.." de "..contents.total_pages )
			if contents.current_page < contents.total_pages then
				stage_surface:blit( arrows.down.x, arrows.down.y, arrows.down.pic ) -- enable
			end
		end
		canvas.flush()
		view_in_cache = true
	end --if status == 1
end

function fillChapter( id, status, data )
	if ( status == 1 ) and stage_showing then -- request ok
		local all_response = jsonParser:decode( data )
		local itemType = categories[nav_bar:getIndex()].type
		if all_response[itemType] and all_response[itemType].id then
			local id = all_response[itemType].id
			if chapterRequests[id] then --existe
				chapterRequests[id].surface:setFont("Tiresias",chapterRequests[id].size)
				local chapters = all_response[itemType].chapters
				if chapters then
					chapterRequests[id].surface:drawText(chapterRequests[id].x, chapterRequests[id].y, 'Capitulos : '..tostring(#chapters))
				else
					chapterRequests[id].surface:drawText(chapterRequests[id].x, chapterRequests[id].y, 'Capitulos : 1')
				end
			end
			canvas.flush()
		end
	end
end

function contentListResetToFirstPage()
	contents.items[ contents.index ].pic:selected( false )
	if ( contents.index > 1 ) then
		contents.items[ contents.index ].pic:draw()
		contents_dirty = true
	end
	contents.last_page_visited = contents.current_page
	contents.current_page = 1
	contents.index = 1
	for i=1,#contents.items_id do
		contents.items_id[ i ] = '0'
	end
	contentListNextPageReset()
end

function contentListNextPageReset()
	for i=1,contents.items_in_page do
		if ( contents.items[ i ].pic:defaultStatus() == false ) then
			contents_dirty = true
			contents.items[ i ].pic:resetDefault()
		end
	end
	contents.items[ contents.index ].pic:selected( true ) -- primer item en la página
	if ( contents_dirty == true ) then
		stage_surface:blit( contents.default_bg.x, contents.default_bg.y, contents.default_bg.pic )
		contents.items[ contents.index ].pic:draw()
	end
	updateArrows()
	contents_dirty = false
end

function contentListNext()
	contents.items[ contents.index ].pic:selected( false )
	if ( contents.index < contents.items_in_page ) then
		contents.items[ contents.index ].pic:draw()
		contents.index = contents.index +1
	else
		if ( contents.current_page < contents.total_pages ) then
			contents.last_page_visited = contents.current_page
			contents.index = 1
			contents.current_page = contents.current_page +1
			contents.request_new_page = true
		end
	end
	contents.items[ contents.index ].pic:selected( true )
	contents.items[ contents.index ].pic:draw()
end

function contentListPrev()
	contents.items[ contents.index ].pic:selected( false )
	if ( contents.index > 1 ) then
		contents.items[ contents.index ].pic:draw()
		contents.index = contents.index -1
	else
		if ( contents.current_page > 1 ) then
			contents.index = contents.items_per_page
			contents.last_page_visited = contents.current_page
			contents.current_page = contents.current_page -1
			contents.request_new_page = true
		end
	end
	contents.items[ contents.index ].pic:selected( true )
	contents.items[ contents.index ].pic:draw()
end

function MainStage:changed( sender )
	sender:load()
	sender:draw()
end

function MainStage:compose( x, y, surface )
	if stage_showing then
		stage_surface:blit( x, y, surface )
		canvas.flush()
	end
end

function MainStage:show()
	stage_showing = true
	if ( view_in_cache == false ) then
		stage_surface:blit( 0, 0, background )
		background:destroy()
		background = nil
		stage_surface:blit( contents.default_bg.x, contents.default_bg.y, contents.default_bg.pic )
		contents.items[ contents.index ].pic:selected( true )
		contents.items[ contents.index ].pic:draw()
		nav_bar:draw( stage_surface )
		canvas.flush()
		contentListRequestNewPage( categories[ nav_bar:getIndex() ].type, categories[ nav_bar:getIndex() ].id )
		view_in_cache = true
	else
		stage_surface:blit( 0, 0, view_cache )
	end
end

local function updateContentList( func, tbl )
	func( tbl )
	tbl:draw( stage_surface )
	contentListResetToFirstPage()
	contentListRequestNewPage( categories[ tbl:getIndex() ].type, categories[ tbl:getIndex() ].id )
end

local function processVideoSource( id, status, data ) -- and play
	if ( status == 1 ) and stage_showing then -- request ok
		local all_response = jsonParser:decode( data )
		local itemType = categories[ nav_bar:getIndex() ].type
		local video = all_response[ itemType ].video
		local streamer = all_response[ itemType ].streamer
		local source = Tools.splitVideoSrc( video.source, '/content/videos/clips/' )
		local url = Tools.makeUrl( streamer, video.type, source, video.id, video.bitrate )
		-- play video
		stage_surface:setVisible( false )
		VideoPlayer.onStop( function() stage_surface:setVisible( true ) end )
		VideoPlayer.playFile( url )
	end
end

local function newPage()
	if contents.request_new_page then
		contentListNextPageReset()
		contentListRequestNewPage( categories[ nav_bar:getIndex() ].type, categories[ nav_bar:getIndex() ].id )
	end
end

function MainStage:leftPressed()
	updateContentList( nav_bar.prev, nav_bar )
end

function MainStage:rightPressed()
	updateContentList( nav_bar.next, nav_bar )
end

function MainStage:enterPressed()
	view_cache:blit( 0, 0, stage_surface )
	local itemIndex = nav_bar:getIndex()
	if ( itemIndex == 5 ) then -- Categoría Igualdad cultural
		if contents.items_id[ contents.index ] ~= '0' then
			client:requestItem( processVideoSource, categories[ itemIndex ].type, contents.items_id[ contents.index ], categories[ itemIndex ].id )
		end
	else
		stage_showing = false
		FiniteStateMachine_doTransition( 'ToItem', contents.items_id[ contents.index ], categories[ itemIndex ].name, categories[ itemIndex ].id, categories[ itemIndex ].type )
	end
end

function MainStage:upPressed()
	contentListPrev()
	newPage()
end

function MainStage:downPressed()
	contentListNext()
	newPage()
end

function MainStage:menuPressed()
end

function MainStage:destroy()
	contents.index = 1
	contents.current_page = 1
	contents.last_page_visited = 1
	view_in_cache = false
	chapterRequests = {}
	-- destroy all surfaces added in the application
	view_cache:destroy()
	arrows.up.pic:destroy()
	arrows.down.pic:destroy()
	arrows.empty.pic:destroy()
	page_counter.bg.pic:destroy()
	contents.default_bg.pic:destroy()
	for i=1,contents.items_per_page do
		contents.items[ i ].pic:destroy()
	end
end