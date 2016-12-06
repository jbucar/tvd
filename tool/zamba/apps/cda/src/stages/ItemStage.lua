-- ITEM STAGE
ItemStage = {
}

local empty_bg
local	item
local	summary
local	more_info
local	pl -- playlist
local nav_bar
local info_frame
local description
local page_counter
local scroll
local help
local category
local background
local stage_surface
local stage_showing
local view_cache
local view_in_cache
local info_selected

setmetatable(ItemStage,Object)
ItemStage.super = Object
ItemStage.__index = ItemStage  -- se prepara para ser Heredada
_G.ItemStage = ItemStage

function ItemStage:Init( mainSurface )
	stage_surface	= mainSurface
	self.keysMap	= KeysMap_getMapForStage( self )
end

local function updateScrollBar( currentPage, totalPages, surface )
	scroll.bar_h = math.floor(( scroll.h - (scroll.offset_bar_y*2) ) / totalPages )
	local y = scroll.offset_bar_y + ( (currentPage -1 ) * scroll.bar_h )
	scroll.pic:setColor( unpack( scroll.bg_color ) )
	scroll.pic:fillRect( 0, 0, scroll.w, scroll.h )
	scroll.pic:setColor( unpack( scroll.bar_color ) )
	scroll.pic:fillRect( scroll.offset_bar_x, y, scroll.bar_w, scroll.bar_h )
	local w, _ = surface:getSize()
	surface:blit( w- scroll.w -4 , 0, scroll.pic )
end

local function next( tbl, func )
	if ( tbl.index < tbl.total ) then
		tbl.index = tbl.index +1
	else
		if ( tbl.index == tbl.total ) then
			tbl.index = 1
		end
	end
	func()
end

local function prev( tbl, func )
	if ( tbl.index > 1 ) then
		tbl.index = tbl.index -1
	else
		if ( tbl.index == 1 ) then
			tbl.index = tbl.total
		end
	end
	func()
end

local function drawItemList( surface, y, txt )
	local x = 10
	surface:drawText( x, y , txt )
end

local function updateSummaryView()
	local i = 1
	local n = summary.lines_per_page
	local summary_bg = info_frame.surfaces[ nav_bar:getIndex() ]
	if summary.index > 1 then
		i = (n * (summary.index-1)) +1
		n = i +(summary.lines_per_page-1)
	end
	local x, y = 10, 10
	local offset_y = info_frame.txt_size
	local summary_bg = info_frame.surfaces[1]
	summary_bg:blit( 0, 0, empty_bg )
	for j=i,n do
		if summary.lines[j] ~= nil then
			summary_bg:drawText( x, y+offset_y , summary.lines[j] )
			offset_y = offset_y +20 -- offset between line
		end
	end
	if summary.total > 1 then
		page_counter.bg:clear()
		page_counter.bg:drawText( 0, page_counter.bg:fontAscent(), 'Pag. '..summary.index..' de '..summary.total )
		summary_bg:blit( page_counter.x, page_counter.y, page_counter.bg )
		if not help.showing_help then
			stage_surface:blit( 250, 527, help.with_pages )
			help.showing_help = true
		end
	else
		stage_surface:blit( 250, 527, help.empty )
	end
	stage_surface:blit( info_frame.x + info_frame.offset_x, info_frame.y + info_frame.offset_y, summary_bg )
end

local function updateMoreInfoView()
	stage_surface:blit( 250, 527, help.empty )
	help.showing_help = false
	stage_surface:blit( info_frame.x + info_frame.offset_x, info_frame.y + info_frame.offset_y, info_frame.surfaces[ nav_bar:getIndex() ] )
end

local function updatePlayListView()
	local pl_bg = info_frame.surfaces[ nav_bar:getIndex() ]
	local current_page = math.ceil(pl.index / pl.per_page)
	local index = pl.index - ( pl.per_page * ( current_page -1 ) )
	pl_bg:blit( 0, 0, empty_bg )
	if ( pl.last_page == current_page ) then
		local y = ( pl.select_rect.h*(index-1) ) + ( pl_bg:fontAscent() - pl_bg:fontDescent() )
		pl_bg:blit( 0, y, pl.select_rect.pic )
		pl_bg:blit( 0, 0, pl.text_pic )
		if ( pl.totalPages > 1 ) then
			updateScrollBar( current_page, pl.totalPages, pl_bg )
		end
	else
		pl.last_page = current_page
		pl.text_pic:clear()
		local n = pl.per_page
		if current_page*pl.per_page > pl.total then
			n = pl.per_page - ( (current_page*pl.per_page) -pl.total )
		end
		local iFunc
		local result = (pl.index == current_page*pl.per_page - (pl.per_page -1))
		if result then -- == primer elemento de la página
			iFunc = function (i,n) return pl.index + (i-1) end
		else
			iFunc = function (i,n) return ( pl.index - n ) + i end
		end
		for i=1, n do
			drawItemList( pl.text_pic, pl.select_rect.h*i ,pl.list_of_chapters[ iFunc(i,n) ] )
		end
		local y = ( pl.select_rect.h*(index-1) ) + ( pl_bg:fontAscent() - pl_bg:fontDescent() )
		pl_bg:blit( 0, y, pl.select_rect.pic )
		pl_bg:blit( 0, 0, pl.text_pic )
		updateScrollBar( current_page, pl.totalPages, pl_bg )
	end
	if not help.showing_help then
		stage_surface:blit( 250, 527, help.with_chapters )
		help.showing_help = true
	end
	stage_surface:blit( info_frame.x + info_frame.offset_x, info_frame.y + info_frame.offset_y, pl_bg )
end

local function drawDescription( titulo, director, playlist )
	local t = Tools.splitText( 40, titulo )
	local offset_y = description.title.font_size
	stage_surface:setColor( 255, 255, 255, 255 )
	stage_surface:setFont( 'Tiresias', description.title.font_size )
	for line in t:gmatch("[^\r\n]+") do
		stage_surface:drawText( description.x, description.y+offset_y , line )
		offset_y = offset_y +20 -- offset between line
	end
	stage_surface:setFont( 'Tiresias', info_frame.txt_size )
	if director ~= '' then
		stage_surface:drawText( description.x, description.y+offset_y , 'Director: '..director )
		offset_y = offset_y +20 -- offset between line
	end
	if playlist and ( #playlist ~= 0 ) then
		stage_surface:drawText( description.x, description.y+offset_y , 'Capítulos: '..#playlist )
	end
end

local function initPlayList( pl_size )
	pl.index = 1
	pl.resources = {}
	pl.text_pic:clear()
	pl.total = pl_size
	pl.totalPages = math.ceil( pl.total/ pl.per_page )
	pl.list_of_chapters = {}
	pl.last_page = 1
	pl.current_page = 1
	if pl.total > 0 then
		if pl_size < pl.per_page then
			pl.inThisPage = pl_size
		else
			pl.inThisPage = pl.per_page
		end
	end
end

local function fillPlayList( list, streamer, type )
	local pl_bg = info_frame.surfaces[ 3 ]
	if ( pl.total > 0 ) then
		info_selected[3] = { updateView = function () updatePlayListView() end, up = function () prev( pl, updatePlayListView ) end, down = function () next( pl, updatePlayListView ) end }
		local url
		local source, bitrate, id
		local title
		for i=1, pl.total do
			if ( type == 'clip' ) then
				source = Tools.splitVideoSrc( list.source, '/content/videos/clips/' )
				bitrate = list.bitrate
				id = list.id
				title = 'Video'
				type = list.type
			else
				source = list[ i ].source
				bitrate = list[ i ].bitrate
				id = list[ i ].id
				title = list[ i ].title
				type = list[ i ].type
			end
			url = Tools.makeUrl( streamer, type, source, id, bitrate )
			pl.resources[ #pl.resources+1 ] = url
			if ( title == '' ) then
				title = 'Capítulo '..i
			else
				title = Tools.processTitle( title, ':', 75 )
			end
			pl.list_of_chapters[ #pl.list_of_chapters+1 ] = title
		end -- for pl.total
		for i=1,pl.per_page do
			drawItemList( pl.text_pic, pl.select_rect.h*i ,pl.list_of_chapters[i] )
		end
		pl_bg:blit( 0, 0, pl.text_pic )
	else
		local r,g,b,a = pl_bg:getColor()
		local offset_x = 10
		local offset_y = 10
		pl_bg:setColor( 178, 178, 178, 255 )
		pl_bg:drawText( offset_x, info_frame.txt_size + offset_y, 'Capitulos no disponibles' )
		pl_bg:setColor( r, g, b, a )
		info_selected[3] = { updateView = function () stage_surface:blit( info_frame.x + info_frame.offset_x, info_frame.y + info_frame.offset_y, info_frame.surfaces[ nav_bar:getIndex() ] ) end, up = function () end, down = function () end }
	end
end

local function processSummary( aSummary )
	aSummary = Tools.trimHtmlStyle(aSummary)
	local x, y = 10, 10
	local offset_y = info_frame.txt_size
	local summary_bg = info_frame.surfaces[ 1 ]
	if ( aSummary ~= nil ) and ( string.len( aSummary ) > 0 ) then
		local text = Tools.splitText( 90, aSummary )
		local lines = {}
		for line in text:gmatch("[^\r\n]+") do
			lines[ #lines+1 ] = line
		end
		summary.total = math.ceil(#lines/summary.lines_per_page)
		summary.lines = lines
		summary.index = 1
		for i=1,summary.lines_per_page do
			summary_bg:drawText( x, y+offset_y , lines[i] )
			offset_y = offset_y +20 -- offset between line
		end
		info_selected[1] = { updateView = function () updateSummaryView() end, up = function () end, down = function () end }
		page_counter.bg:clear()
		if summary.total > 1 then
			page_counter.bg:drawText( 0, page_counter.bg:fontAscent(), 'Pag. '..summary.index..' de '..summary.total )
			stage_surface:blit( 250, 527, help.with_pages )
			info_selected[1] = { updateView = function () updateSummaryView() end, up = function () prev( summary, updateSummaryView ) end, down = function () next( summary, updateSummaryView ) end }
		end
		summary_bg:blit( page_counter.x, page_counter.y, page_counter.bg )
	else
		local r,g,b,a = summary_bg:getColor()
		summary_bg:setColor( 178, 178, 178, 255 )
		summary_bg:drawText( x, y+offset_y , 'Sinopsis no disponible' )
		summary_bg:setColor( r, g, b, a )
		info_selected[1] = { updateView = function () stage_surface:blit( info_frame.x + info_frame.offset_x, info_frame.y + info_frame.offset_y, info_frame.surfaces[ nav_bar:getIndex() ] ) end, up = function () end, down = function () end }
	end
end

local function processMoreInfo( actors, guion, plan )
	local x, y = 10, 10
	local offset_y = info_frame.txt_size
	local more_infoBg = info_frame.surfaces[ 2 ]
	info_selected[2] = { updateView = function () updateMoreInfoView() end, up = function () end, down = function () end }
	if ( actors ~= nil ) and ( string.len( actors ) > 0 ) then
		local text = Tools.splitText( 90, actors )
		local lines = {}
		more_infoBg:setColor( 255, 255, 255, 255 )
		more_infoBg:drawText( x, y+offset_y , 'ACTORES:' )
		offset_y = offset_y +20 -- offset between line
		more_infoBg:setColor( 178, 178, 178, 255 )
		for line in text:gmatch("[^\r\n]+") do
			lines[ #lines+1 ] = line
		end
		for i=1,#lines do
			more_infoBg:drawText( x, y+offset_y , lines[i] )
			offset_y = offset_y +20 -- offset between line
		end
	elseif ( actors ~= nil ) and ( string.len( actors ) > 0 ) then
		more_infoBg:setColor( 255, 255, 255, 255 )
		more_infoBg:drawText( x, y+offset_y , 'GUIÓN:' )
		offset_y = offset_y +20 -- offset between line
		more_infoBg:setColor( 178, 178, 178, 255 )
		more_infoBg:drawText( x, y+offset_y , guion )
	elseif ( plan ~= nil ) and ( string.len( plan ) > 0 ) then
		more_infoBg:setColor( 255, 255, 255, 255 )
		more_infoBg:drawText( x, y+offset_y , 'PLAN:' )
		offset_y = offset_y +20 -- offset between line
		more_infoBg:setColor( 178, 178, 178, 255 )
		more_infoBg:drawText( x, y+offset_y , plan )
	else
		info_selected[2] = { updateView = function () updateMoreInfoView() end, up = function () end, down = function () end }
		local r,g,b,a = more_infoBg:getColor()
		more_infoBg:setColor( 178, 178, 178, 255 )
		more_infoBg:drawText( x, y+offset_y , 'Información no disponible' )
		more_infoBg:setColor( r, g, b, a )
	end
end

local function processPlayList( tbl, streamer, type )
	local list
	if type == 'clip' then
		list = tbl.video
		initPlayList( 1 )
	else
		list = tbl.chapters
		initPlayList( #list or 0 )
	end
	fillPlayList( list, streamer, type )
end

local function processResult( id, status, data )
	if status == 1 and stage_showing then -- request ok
		local all_response = jsonParser:decode( data )
		if all_response[ item.type ] then -- request ok
			local image = all_response[ item.type ].image
			if ( image ~= nil ) then
				item.pic:fetch( 'http://api.prod.cda.dcarsat.com.ar'..image.source..image.id..image.sizes[ 1 ].subfix..'.'..image.type )
			end
			for i=1,#info_frame.surfaces do -- clean surfaces
				info_frame.surfaces[ i ]:blit( 0, 0, empty_bg )
			end
			processSummary(	all_response[ item.type ].summary )
			processMoreInfo( all_response[ item.type ].actors, all_response[ item.type ].guion, all_response[ item.type ].plan )
			processPlayList( all_response[ item.type ], all_response[ item.type ].streamer, item.type )
			drawDescription( all_response[ item.type ].title, all_response[ item.type ].director, all_response[ item.type ].chapters )
			stage_surface:blit( info_frame.x + info_frame.offset_x, info_frame.y + info_frame.offset_y, info_frame.surfaces[ nav_bar:getIndex() ] )
			canvas.flush()
		end
 	end
end

local function updateInfoFrame( func, tbl )
	func( tbl )
	tbl:draw( stage_surface )
	help.showing_help = false
	info_selected[ tbl:getIndex() ].updateView()
	canvas.flush()
end

function ItemStage:leftPressed()
	updateInfoFrame( nav_bar.prev, nav_bar )
end

function ItemStage:rightPressed()
	updateInfoFrame( nav_bar.next, nav_bar )
end

function ItemStage:enterPressed()
	local urlChapter = pl.resources[ pl.index ]
	if urlChapter and ( nav_bar:getIndex() == 3 ) then
		stage_surface:setVisible( false )
		VideoPlayer.onStop( function() stage_surface:setVisible( true ) end )
		VideoPlayer.playFile( urlChapter )
	end
end

function ItemStage:menuPressed()
	stage_showing = false
	nav_bar:draw( stage_surface )
	view_cache:blit( info_frame.x + info_frame.offset_x, info_frame.y + info_frame.offset_y, info_frame.surfaces[ nav_bar:getIndex() ] )
	view_cache:blit( 0, 0, stage_surface )
	FiniteStateMachine_doTransition('ToMain')
end

function ItemStage:upPressed()
	info_selected[ nav_bar:getIndex() ].up()
	canvas.flush()
end

function ItemStage:downPressed()
	info_selected[ nav_bar:getIndex() ].down()
	canvas.flush()
end

function ItemStage:changed( sender )
	sender:load()
	sender:draw()
end

function ItemStage:compose( x, y, surface )
	if stage_showing then
		stage_surface:blit( x, y, surface )
		canvas.flush()
	end
end

function ItemStage:load()
	item = { x = 52, y = 115, pic = '', id = '-1', category_id = '', type = '' }
	summary = { lines_per_page = 7, index = 1, total = 0, lines = {} }
	more_info = {}
	pl = {
		text_pic = '',
		list_of_chapters = '',
		index = 1, total = 1, per_page = 6, last_page = 1, current_page = 1,
		resources = {},
		select_rect = { pic = '', color = {19, 154, 233, 200}, w = 584, h = 25 }
	}
	info_frame = { x = 52, y = 265, offset_x = 1, offset_y = 50, bg = '', surfaces = {}, txt_size = 14 }
	description = { x = 300, y = 130, title = { font_size = 16, title = '' }, director = '', chapters = '' }
	page_counter = { x = 490, y = 155, bg = '' }
	scroll = { x = 310, y = 305, w = 20, h = 176, bg_color = { 112, 112, 112, 255 }, pic = '', bar_color = { 51, 51, 51, 255 }, bar_w = 16, bar_h = 0, offset_bar_x = 2, offset_bar_y = 2 }
	help = { default = '', empty = '', with_pages = '', with_chapters = '', showing_help = false }
	category = { x = 120, y = 60 }
	stage_showing = false
	view_in_cache = false
	info_selected = {}
	background	= canvas.createSurfaceFromPath( 'resources/images/background2.png' )
	view_cache = canvas.createSurface( 0, 0, background:getSize() )
	help.default = canvas.createSurfaceFromPath( 'resources/images/Ayuda4.png' )
	help.empty = canvas.createSurfaceFromPath( 'resources/images/AyudaVacio.png' )
	help.with_pages = canvas.createSurfaceFromPath( 'resources/images/Ayuda5.png' )
	help.with_chapters = canvas.createSurfaceFromPath( 'resources/images/Ayuda6.png' )
	background:blit( 0, 517, help.default )
	local w, h = 620, 177
	info_frame.bg = canvas.createSurfaceFromPath('resources/images/infoFrame.png')
	scroll.pic = canvas.createSurface( 0, 0, scroll.w, scroll.h )
	empty_bg = canvas.createSurface( 0, 0, w, h )
	empty_bg:setColor( 51, 51, 51, 255 )
	empty_bg:fillRect( 0, 0, w, h )
	info_frame.surfaces[1] = canvas.createSurface( 0, 0, w, h ) -- summary surface
	info_frame.surfaces[1]:setFont( 'Tiresias', info_frame.txt_size )
	info_frame.surfaces[1]:setColor( 255, 255, 255, 255 )
	info_frame.surfaces[2] = canvas.createSurface( 0, 0, w, h ) -- more_info surface
	info_frame.surfaces[2]:setFont( 'Tiresias', info_frame.txt_size )
	info_frame.surfaces[2]:setColor( 255, 255, 255, 255 )
	info_frame.surfaces[3] = canvas.createSurface( 0, 0, w, h ) -- chapters surface
	info_frame.surfaces[3]:setFont( 'Tiresias', info_frame.txt_size )
	info_frame.surfaces[3]:setColor( 255, 255, 255, 255 )
	local rect = pl.select_rect
	rect.pic = canvas.createSurface( 0, 0, rect.w, rect.h )
	rect.pic:setColor( unpack( rect.color ) )
	rect.pic:fillRect( 0, 0, rect.w, rect.h )
	pl.text_pic = canvas.createSurface( 0, 0, w, h )
	pl.text_pic:setFont( 'Tiresias', info_frame.txt_size )
	pl.text_pic:setColor( 255, 255, 255, 255 )
	page_counter.bg = canvas.createSurface( 0, 0, 80, 17 ) -- según measureText del peor caso
	page_counter.bg:setFont( 'Tiresias', 13 )
	page_counter.bg:setColor( 178, 178, 178, 255 )
	-- LOADING NAVIGATION BAR
	local items = {}
	items[1] = { x = 66, y = 294, w = 60, h = 3 }
	items[2] = { x = 148, y = 294, w = 130, h = 3 }
	items[3] = { x = 298, y = 294, w = 75, h = 3 }
	nav_bar = NavigationBar:new( items )
	item.pic = RemoteImage:new( item.x, item.y, 'resources/images/itemDefaultBgBig.png' )
	item.pic:setContainer( self )
	item.pic:load()
	info_selected[1] = { updateView = function () end, up = function () end, down = function () end }
	info_selected[2] = { updateView = function () end, up = function () end, down = function () end }
	info_selected[3] = { updateView = function () end, up = function () end, down = function () end }
end

function ItemStage:show( id, name, category_id, type )
	stage_showing = true
	if ( item.id ~= id ) then
		item.category_id = category_id
		item.id = id
		item.type = type
		for i=1,#info_frame.surfaces do -- clean surfaces
			info_frame.surfaces[ i ]:blit( 0, 0, empty_bg )
		end
		stage_surface:blit( 0, 0, background )
		item.pic:resetDefault()
		item.pic:draw()
		stage_surface:blit( info_frame.x, info_frame.y, info_frame.bg )
		if ( item.id ~= '0' ) then
			stage_surface:setFont( 'Tiresias', 14 )
			stage_surface:setColor( 255, 255, 255, 255 )
			stage_surface:drawText( category.x, category.y, name )
		end
		help.showing_help = false
		nav_bar:reset()
		nav_bar:draw( stage_surface )
		canvas.flush()
		client:requestItem( processResult, item.type, item.id, item.category_id )
	else
		stage_surface:blit( 0, 0, view_cache )
	end
end

function ItemStage:destroy()
	background:destroy()
	view_cache:destroy()
	info_frame.bg:destroy()
	item.pic:destroy()
	scroll.pic:destroy()
	for i=1,#info_frame.surfaces do
		info_frame.surfaces[ i ]:destroy()
	end
	empty_bg:destroy()
	pl.select_rect.pic:destroy()
end
