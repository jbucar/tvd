local feedsColor = 'white'

function clearFeeds()
	canvas:attrColor(0,0,0,0)
	canvas:clear(100,270,620,25)
	canvas:attrColor(feedsColor)
end	

function setFontProperties()
	canvas:attrColor(feedsColor)
	canvas:attrFont('Tiresias',20,nil)
end

function drawFeeds(feeds)
	clearFeeds()
	canvas:drawText(100,270,feeds)
	canvas:flush()
end

function handler(evt)
	if evt.class == 'ncl' and evt.type == 'presentation' then
		setFontProperties()
		drawFeeds("Waiting for new feeds...")
	elseif evt.class =='ncl' and evt.type == 'attribution' then
		drawFeeds(evt.value)
	end
end

event.register(handler)