function handler(evt)
	if evt.class=='key' and evt.type=='press' then 
		if evt.key=='F2' or evt.key=='GREEN' then
			event.post('out', {class = 'ncl',
			 			type = 'presentation',
			 			label = 'area',
			 			action = 'stop',
			 			})
		end
		if evt.key=='F1' or evt.key=="RED" then
			event.post('out', {class = 'ncl',
			 			type = 'presentation',
			 			label = 'area',
			 			action = 'start',
			 			})
		end
	end
end

event.post('out', {class = 'ncl',
 			type = 'presentation',
 			label = 'area',
 			action = 'start',
 			})

event.register(handler)
