--- Class RemoteImage
local DEFAULT = 1
local SELECTED = 2
local UNSELECTED = 3

RemoteImage	=	{
	container	=	nil,
	surfaces = {},
	status = 1,
	path	= '',
	is_updated = false,
}

setmetatable(RemoteImage,Object)
RemoteImage.super   = Object
RemoteImage.__index = RemoteImage

--- Class RemoteImage constructor
function RemoteImage:Init( x, y, path )
	self.x = x
	self.y = y
	self.is_updated = false
	self.path = path
	self.surfaces = {}
	self.surfaces[ DEFAULT ] = { path = path, pic = nil }
	self.surfaces[ SELECTED ] = { path = '', pic = nil }
	self.surfaces[ UNSELECTED ] = { path = '', pic = nil }
	self.status = DEFAULT
	self.is_default = true -- true si las surfaces SELECTED y UNSELECTED tienen el mismo contenido que la surface DEFAULT
end

function RemoteImage:destroy()
	self.surfaces[ DEFAULT ].pic:destroy()
	self.surfaces[ SELECTED ].pic:destroy()
	self.surfaces[ UNSELECTED ].pic:destroy()
end

--ACCESSORS
function RemoteImage:setContainer( aContainer )
	self.container = aContainer
end

--- Load RemoteImage in memory.
function RemoteImage:load()
	if ( self.status == DEFAULT ) then
		if self.surfaces[ DEFAULT ].pic == nil then
			self.surfaces[ DEFAULT ].pic = canvas.createSurfaceFromPath( self.surfaces[ DEFAULT ].path )
		end
		local w, h = self.surfaces[ DEFAULT ].pic:getSize()
		self.surfaces[ UNSELECTED ].pic = canvas.createSurface( 0, 0, w, h )
		self.surfaces[ UNSELECTED ].pic:blit( 0, 0, self.surfaces[ DEFAULT ].pic )
		self.surfaces[ SELECTED ].pic = canvas.createSurface( 0, 0, w, h )
		self.surfaces[ SELECTED ].pic:blit( 0, 0, self.surfaces[ DEFAULT ].pic )
		self.surfaces[ SELECTED ].pic:setColor( 19, 154, 233, 255 )
		self:drawBorder( 3, self.surfaces[ SELECTED ].pic )
	end
	if self.is_updated then
		local unselected = canvas.createSurfaceFromPath( self.path )
		local w, h = self.surfaces[ DEFAULT ].pic:getSize()
		unselected:resize( w, h, true )
		self.surfaces[ UNSELECTED ].pic:blit( 0, 0, unselected )
		self.surfaces[ SELECTED ].pic:blit( 0, 0, unselected )
		unselected:destroy()
		self:drawBorder(3, self.surfaces[ SELECTED ].pic )
		if self.status == DEFAULT then
			self.status = UNSELECTED
		end
		self.is_default = false
		self.is_updated = false
	end
end

--- Draw the RemoteImage in screen.
function RemoteImage:draw()
	self.container:compose( self.x, self.y, self.surfaces[ self.status ].pic )
end

function  RemoteImage:defaultStatus()
	return self.is_default
end

function RemoteImage:drawBorder( size, surface )
	local w, h = surface:getSize()
	local x, y = 0, 0
	for i=1,size do
		surface:drawRect( x, y, w-(x*2), h-(y*2) )
		x = x +1
		y = y +1
	end
end

function RemoteImage:resetDefault()
	self.surfaces[ UNSELECTED ].pic:blit( 0, 0, self.surfaces[ DEFAULT ].pic )
	self.surfaces[ SELECTED ].pic:blit( 0, 0, self.surfaces[ DEFAULT ].pic )
	self:drawBorder( 3, self.surfaces[ SELECTED ].pic )
	self.is_default = true
end

function RemoteImage:setPath( aPath )
	self.path = aPath
	if self.container then
		self.container:changed( self )
	end
end

function RemoteImage:selected( aBoolean )
	if aBoolean then
		self.status = SELECTED
	else
		self.status = UNSELECTED
	end
end

function RemoteImage:fetch( url )
	local fnc = function ( id, status, newPath ) self:processResult( id, status, newPath ) end --callback fetch images
	client:fetchResource( fnc, url )
end

function RemoteImage:processResult( id, status, newPath )
	if ( status == 1 ) then
		self.is_updated = true
		self:setPath( newPath )
	else
		self.is_updated = false
	end
end