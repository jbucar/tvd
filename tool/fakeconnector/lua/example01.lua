--Ejemplo para la aplicación partido:
--https://svn.tecnologia.lifia.info.unlp.edu.ar:6002/svn/rep023_GingaApplications/trunk/src/partido_feeds/

t = 500 --ms

sleep(t)
connector.sendKey(INFO)

sleep(t)
connector.sendKey(RED)

sleep(t)
connector.sendKey(GREEN)

sleep(t)
connector.sendKey(YELLOW)

sleep(t)
connector.sendKey(BLUE)

sleep(t)
value = ""
for i=1, 10 do
	value = value .."Test "..i.."<!>"
	connector.sendPropertyValueEC("feeds","app","feeds",value)  -- appId, mediaId, propertyId, value
	sleep(t)
end

sleep(t)
connector.sendKey(INFO)

sleep(t)




