local interactivityOn=false

function attributionEventDispatch(action, property, value)
	return 	event.post('out',{class = "ncl", type = "attribution", name = property, action=action, value=value})
end

function dispatchEvent(property, value)
	attributionEventDispatch('start', property, value)
	attributionEventDispatch('stop',  property, value)
end

dispatchEvent("itemChange", "INFO")
function handler(evt)



	if evt.class =='key' and evt.type=='press' then	

		if (evt.key =='ENTER' or evt.key =='OK' ) then
			
			interactivityOn= not interactivityOn
			if interactivityOn then 
				dispatchEvent("itemChange", "INITIAL")
			else
				dispatchEvent("itemChange", "INFO")
			end
		end
		
		
		if interactivityOn and (evt.key == "RED" or evt.key =='F1')  then
			dispatchEvent("itemChange","RED")
		end	
		if interactivityOn and (evt.key == "GREEN" or evt.key =='F2') then 
			dispatchEvent("itemChange","GREEN")
		end	
		if interactivityOn and (evt.key== "YELLOW" or evt.key =='F3') then
			dispatchEvent("itemChange","YELLOW")
		end	
		if interactivityOn and (evt.key== "BLUE" or evt.key =='F4') then
			dispatchEvent("itemChange","BLUE")
		end
	end	 
end



event.register(handler)




