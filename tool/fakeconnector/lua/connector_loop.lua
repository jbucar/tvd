function OnGinga( isRunning )
	print( "[lua] Ginga running: " .. isRunning )
end

function OnConnected( isConnected )
	print( "[lua] Connected: " .. isConnected )
	if (isConnected == 0) then
		connector.exit()
	end		
end

function OnKeyEvent( key )
	print( "[lua] Key pressed: " .. key )
end

print("[lua] Begin")
connector.run()
print("[lua] End")