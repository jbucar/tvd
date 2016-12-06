delay = 1000 --miliseconds
sleep(delay)
value = ""


function OnGinga( isRunning )
	print( "[lua] Ginga running: " .. isRunning )
	if isRunning then
		sleep(delay)
		for i=1, 7 do
			value = value .." Feed "..i
			connector.sendPropertyValueEC("feedsPublica","app","feeds",value)  -- appId, mediaId, propertyId, value
			sleep(delay)
		end
		sleep(delay)
		connector.sendPropertyValueEC("feedsPublica","app","feeds","Feeds enviados exitosamente")
	end
end

function OnConnected( isConnected )
	print( "[lua] Connected: " .. isConnected )
end

function OnKeyEvent( key )
	print( "[lua] Key pressed: " .. key )
end

connector.run()
print("[lua] End")