--Information: 
	-- Source: 294 (1.0.0-294-g118c7f9)
	-- Config: TYPE=DEBUG;SHARED=OFF;GUI=GTKCAIRO;HTML_RENDER=NOTHING
print("[lua] Begin")
colours = {RED,YELLOW,BLUE,GREEN}
menu = {CURSOR_DOWN,CURSOR_UP,CURSOR_RIGHT,CURSOR_LEFT}

delay = 1000 --ms
cantRepeat = 20
stageInfo = true

iterations = 0

math.randomseed( os.time() )

function OnGinga( isRunning )
	print( "[lua] Ginga running: " .. isRunning )
	if isRunning then
		-- Se envía el botón info
		sleep(delay)
		connector.sendKey(INFO)
		sleep(delay)

		while true do
			iterations = iterations + 1
			print("[lua] Iteration "..iterations)

			--Se envían teclas de colores al azar 
			-- toPress = math.random(1,#colours)
			-- for i=1,cantRepeat do
				--   connector.sendKey(colours[toPress])  
			-- end  


			--Se envían teclas de colores al azar 
			for i=1,cantRepeat do
				toPress = math.random(1,#colours)
				connector.sendKey(colours[toPress])  
				sleep(delay)
			end  

			--Se escoge una tecla al azar y se envía repetidas veces
			toPress = math.random(1,#colours)
			for i=1,cantRepeat do
				connector.sendKey(colours[toPress])  
				sleep(delay)
			end

			--Se envía la tecla info repetidas veces
			for i=1,cantRepeat do
				connector.sendKey(INFO)  
				sleep(delay)
				stageInfo = not stageInfo
			end
			
			if not stageInfo then
				print("Cambiando a stage INfo")
				stageInfo = true
				connector.sendKey(INFO)
			end
		end
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





