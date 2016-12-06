--Information: 
	-- Source: 294 (1.0.0-294-g118c7f9)
	-- Config: TYPE=DEBUG;SHARED=OFF;GUI=GTKCAIRO;HTML_RENDER=NOTHING

print("[lua] Begin")
colours = {RED,YELLOW,BLUE,GREEN}
menu = {CURSOR_DOWN,CURSOR_UP,CURSOR_RIGHT,CURSOR_LEFT}
sequence = {CURSOR_UP,CURSOR_DOWN,CURSOR_UP,CURSOR_DOWN,CURSOR_DOWN,CURSOR_DOWN,CURSOR_UP,CURSOR_UP,CURSOR_LEFT,CURSOR_RIGHT,CURSOR_RIGHT,CURSOR_LEFT,CURSOR_RIGHT,CURSOR_RIGHT,CURSOR_RIGHT,CURSOR_LEFT,CURSOR_LEFT,CURSOR_LEFT}

delay = 1000 --ms
amountOfRepetitions = 20
stageInfo = true

math.randomseed( os.time() )
iterations = 0

function OnGinga( isRunning )
	print( "[lua] Running: " .. isRunning )
	if isRunning then
		-- Se envía el botón info
		print("[lua] Sending Info button")
		sleep(delay)
		connector.sendKey(INFO)
		sleep(delay)
		while true do
			iterations = iterations + 1
			print("[lua] Iteration "..iterations)

			--Se envían teclas de colores al azar 
			for i=1,amountOfRepetitions do
				toPress = math.random(1,#colours)
				connector.sendKey(colours[toPress])  
				sleep(delay)
			end  

			--Se escoge una tecla al azar y se envía repetidas veces
			toPress = math.random(1,#colours)
			for i=1,amountOfRepetitions do
				connector.sendKey(colours[toPress])  
				sleep(delay)
			end

			-- Se envía el botón menu
			connector.sendKey(MENU)

			--Se envían teclas de navegacion (flechas) al azar
			for i=1,amountOfRepetitions do
				toPress = math.random(1,#menu)
				connector.sendKey(menu[toPress])  
				sleep(delay)
			end

			--Se envía una secuencia pre establecida de botones
			for i,toPress in ipairs(sequence) do
				connector.sendKey(toPress)
				sleep(delay)
			end

			--Se envía repetidamente la tecla INFO
			for i=1,amountOfRepetitions do
				connector.sendKey(INFO)  
				stageInfo = not stageInfo
				sleep(delay)
			end

			--Se envía repetidamente la tecla INFO
			for i=1,amountOfRepetitions do
				connector.sendKey(MENU)  
				sleep(delay)
			end

			--Se envía repetidamente la tecla INFO y MENU intercaladas
			for i=1,amountOfRepetitions do
				connector.sendKey(MENU)  
				sleep(delay)
				connector.sendKey(INFO)
				stageInfo = not stageInfo
				sleep(delay)
			end

			if not stageInfo then
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




