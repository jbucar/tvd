--- Class Client
Client = {
	-- Instance Atributes
}

setmetatable(Client,Object)
Client.super   = Object
Client.__index = Client

--- Class initialization method
function Client:Init( anUrl, anApikey )
	self.file_path = 'resources/images/itemDefaultBg.png'
	self.http_req = {} -- requests
	self.url_cache = {}
	self.fetch_cache = {}
	self.request = {} --al menos seis peticiones por página
	self.request.responde_type = 'json'
	self.request.url = anUrl
	self.request.headers = { 'apikey: ' .. anApikey }
end

function Client:cancelHttpRequests() --cancel only http requests
	local requests = #self.http_req
	if requests > 0 then
		for i=1,requests do
			net.cancel( self.http_req[ i ] )
		end
	end
	self.http_req = {}
end

function Client:cancelFetchRequests()
	for id,_ in pairs(self.fetch_cache) do
		net.cancel(id)
	end
	self.url_cache = {}
	self.fetch_cache = {}
end

function Client:cancelAllRequests()
	self:cancelHttpRequests()
	self:cancelFetchRequests()
end

function Client:post( aParam, callbackF )
	aParam = aParam..'&response='..self.request.responde_type
	local id = net.httpRequest{
	 	url = self.request.url,
		method = 'POST',
	 	headers = self.request.headers,
		body = aParam,
		callback = callbackF,
		verbose = 0
	}
	self.http_req[ #self.http_req+1 ] = id
end

function Client:requestList( callbackF, aType, aCategory_id, perPage, currentPage )
	local param = 'call=list&type='..aType..'&category_id='..aCategory_id..'&perPage='..perPage..'&currentPage='..currentPage
	self:post(param, callbackF)
end

function Client:requestItem( callbackF, aType, item_id, category_id )
	local param = 'call=item&category_id='..category_id..'&type='..aType..'&item_id='..item_id
	self:post( param, callbackF )
end

function Client:cacheCallback( id, status, newPath )
	if ( status == 1 ) then
		local cache = self.fetch_cache[ id ]
		cache.file = newPath
		cache.status = status
		cache.callbackF( id, status, newPath )
	end
end

function Client:fetchResource( callbackF, url )
	local id = self.url_cache[ url ]
	if ( id ~= nil ) and ( self.fetch_cache[ id ].status ~= -1 ) then
		callbackF( id, self.fetch_cache[ id ].status, self.fetch_cache[ id ].file )
	else -- not in cache ( status ~= -1, 0, 1 )
		local id = net.fetch{
			url = url,
			method = 'POST',
			headers = self.request.headers,
			body = aParam,
			callback = function ( id, status, newPath )  self:cacheCallback( id, status, newPath ) end,
			verbose = 0
		}
		self.url_cache[ url ] = id
		self.fetch_cache[ id ] = { file = self.file_path, status = -1, callbackF = callbackF }
	end
end