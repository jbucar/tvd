NavigationBar = {
	items = {},
	last_i = 1,
	i = 1, -- index 
}

setmetatable(NavigationBar,Object)
NavigationBar.super   = Object
NavigationBar.__index = NavigationBar

function NavigationBar:Init( items )
	self.items = items
end

function NavigationBar:next( func )
	self.last_i = self.i
	if ( self.i < #self.items ) then
		self.i = self.i +1
	else
		if ( self.i == #self.items ) then
			self.i = 1
		end
	end
end

function  NavigationBar:prev( func )
	self.last_i = self.i
	if ( self.i > 1 ) then
		self.i = self.i -1
	else
		if ( self.i == 1 ) then
			self.i = #self.items
		end
	end
end

function NavigationBar:draw( aSurface )
	local r,g,b,a = aSurface:getColor()
	local item
	aSurface:setColor( 65, 65, 65, 255 )
	item = self.items[ self.last_i ]
	aSurface:fillRect( item.x, item.y, item.w, item.h )
	aSurface:setColor( 19, 154, 233, 200 )
	item = self.items[ self.i ]
	aSurface:fillRect( item.x, item.y, item.w, item.h )
	aSurface:setColor( r, g, b, a )
end

function NavigationBar:getIndex()
	return self.i
end

function NavigationBar:reset()
	self.i = 1
end