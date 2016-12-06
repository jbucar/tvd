local recetaIndex = 0
local ingredientesIndex = 0

function dibujarIngredientes(index)
  canvas:compose(0,0,canvas:new("resources/images/Recetas_Ingredientes"..(index+1)..".png"))
  canvas:flush()
end  

function dibujarReceta(index)
  canvas:compose(0,375,canvas:new("resources/images/Recetas_Preparacion"..(index+1)..".png"))
  canvas:flush()
end  

function handler(evt)
	if evt.class=="ncl" and evt.type=="attribution" then
		if evt.value == "up" and ingredientesIndex ~= 0 then
			ingredientesIndex = (ingredientesIndex - 1) % 3
			dibujarIngredientes(ingredientesIndex)
		end	
		
		if evt.value == "down" and ingredientesIndex ~= 2 then
			ingredientesIndex = (ingredientesIndex + 1) % 3
			dibujarIngredientes(ingredientesIndex)
		end
		if evt.value == "left"  and recetaIndex ~= 0 then
			recetaIndex = (recetaIndex - 1) % 4
			dibujarReceta(recetaIndex)
		end	
		if evt.value == "right" and recetaIndex ~= 3 then
			recetaIndex = (recetaIndex + 1) % 4
			dibujarReceta(recetaIndex)
		end	
	end	
end

dibujarIngredientes(ingredientesIndex)
dibujarReceta(recetaIndex)
event.register(handler)




