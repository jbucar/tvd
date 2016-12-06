function handler(evt)
	if evt.class =='key' and evt.type =='press' then
		if evt.key == 'F1' or evt.key == 'RED' then
			print("F1")
			event.post('out',{class = "ncl", type = "attribution", name = 'srcNueva', action='start', value='../resources/images/image9.jpg'})
			event.post('out',{class = "ncl", type = "attribution", name = 'srcNueva', action='stop', value='../resources/images/image9.jpg'})

		else
			if evt.key == 'F2' or evt.key=='GREEN' then
			event.post('out',{class = "ncl", type = "attribution", name = 'srcNueva', action='start', value='../resources/images/image8.jpg'})
			event.post('out',{class = "ncl", type = "attribution", name = 'srcNueva', action='stop', value= '../resources/images/image8.jpg'})

			end
		end
	end
end

event.register(handler)
