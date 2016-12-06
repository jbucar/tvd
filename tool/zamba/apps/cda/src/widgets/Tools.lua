Tools = {}

function Tools.makeUrl( streamer, type, source, id, bitrate )
	local host = '186.33.226.132/vod'
	local protocol = 'rtmp'
	if (streamer == nil or streamer == '') then
		streamer = host
	else
		streamer = Tools.splitVideoSrc( streamer, host)
	end
	return protocol..'://'..streamer..'/_definst_/'..type..':'..source..id..'_'..bitrate..'.'..type
end

function Tools.splitVideoSrc( str, pattern )
	local i = string.find( str, pattern )
	local result = ''
	if i ~= nil then
		result = string.sub( str, i )
	end
	return result
end

function Tools.resize( str )
	local limit = 35
	if string.len( str ) > limit then
		return string.sub(str, 0, limit)..'...'
	else
		return str
	end
end

function Tools.splitText( limit, str )
	limit = limit or 72
	local here = 1
	return str:gsub("(%s+)()(%S+)()",function(sp, st, word, fi) if fi-here > limit then here = st	return "\n"..word end end)
end

function Tools.processTitle( str, pattern, maxLength )
	local res = {}
	for w in str:gmatch('[^'..pattern..']+') do
		res[#res + 1] = w
	end
	if #res > 1 then
		res[1] = ''
	end
	local title = table.concat( res )
	if string.len(title) >= maxLength then
		title = string.sub(title,1,maxLength-3)..'...'
	end
	return title
end

local function trim( s, o, c, t )
	local k
	while s ~= '' do
		j = string.find(s,o) -- open
		if j~= nil and j > 1 then
			t[ #t+1 ] = string.sub(s,1,j-1)
		end
		k = string.find(s,c)-- close
		if k ~= nil then
			s = string.sub( s, k+string.len(c),string.len(s) )
		else
			t[ #t+1 ] = s
			s = ''
		end
	end
end

function Tools.trimHtmlStyle( str )
	local o1,c1 = "&lt;", "&gt;" -- open , close
	local o2,c2 = "<", ">" -- open , close
	local t = {}
	str = string.gsub(str,o1,'<')
	str = string.gsub(str,c1,'>')
	trim( str, o2, c2, t )
	return table.concat(t)
end

function  Tools.assert(anBoolean, message)
	print(message)
	return anBoolean
end