

-- usamos una variable para saber si la animacion esta ejecutándose
local runAnimation = false

local cancelFunction = nil

local images={
						canvas:new('loadingImages/FRAME_000001.png'),canvas:new('loadingImages/FRAME_000002.png')
					,canvas:new('loadingImages/FRAME_000003.png'),canvas:new('loadingImages/FRAME_000004.png')
					,canvas:new('loadingImages/FRAME_000005.png'),canvas:new('loadingImages/FRAME_000006.png')
					,canvas:new('loadingImages/FRAME_000007.png'),canvas:new('loadingImages/FRAME_000008.png')
													}

local actualIndex=1
local actualImage= images[actualIndex]


function updateAnimation()

	if runAnimation == true then

		if actualIndex < #images then
-- 			pasamos a la sgte imagen en la tabla actualizando el indice
				actualIndex = actualIndex+1
		else
-- si se nos terminan la imagenes que mostramos volvemos a inicio de la tabla
				actualIndex =1
		end
		draw()
		cancelFunction = event.timer(100,updateAnimation)

	end
	
end

function draw()

	actualImage = images[actualIndex]
-- 	componemos la imagen en el centro de la imagen de fondo
	canvas:compose(12,4,actualImage)
	canvas:flush()
	
end


function startAnimation()
	print("Timer started")

-- 	creamos una imagen de fondo
	local background= canvas:new('loadingImages/fondoLoading.png')
-- componemos la imagen de fondo una sola vez
	canvas:compose(0,0,background)
-- actualizamos la pantalla
	canvas:flush()
	
-- 	dibujamos la imagen de cargando
	draw()
	
	runAnimation = true
	cancelFunction = event.timer(100,updateAnimation)
	
end

function stopAnimation()
	if (cancelFunction ~= nil) then 
		cancelFunction()
-- 		limpiamos la región asociada a Lua con magenta (es el color transparente que utiliza Lua)
		canvas:attrColor(255,0,255,0)
		canvas:clear()
		
		canvas:flush()
		runTimer=false
		
		print("Timer stopped")
	end
end



function handler(evt)

	if evt.name=='state' and evt.type=='attribution' and evt.value=='start' then
-- 		detenemos la animación por si estuviera corriendo previamente
		stopAnimation()
-- iniciamos la animación
		startAnimation()
	end
  
		if evt.name=='state' and evt.type=='attribution' and evt.value=='stop'then
-- 		detenemos la animación
		stopAnimation()
		end
end

event.register(handler)





