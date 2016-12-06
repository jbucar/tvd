-- Testea que #attrFont retorne correctamente la fuente, el tamaño y el estilo.

canvas:attrFont("Verdana",20,'bold')
canvas:attrColor("blue")
fuente,tamanio,estilo = canvas:attrFont()
texto = "Fuente: "..fuente..". Tamanio "..tamanio..". Estilo: "..estilo
canvas:drawText(10,0,texto)

canvas:attrFont("Verdana",20,'bold-italic')
canvas:attrColor("blue")
fuente,tamanio,estilo = canvas:attrFont()
texto = "Fuente: "..fuente..". Tamanio "..tamanio..". Estilo: "..estilo
canvas:drawText(10,30,texto)

canvas:attrFont("Verdana",10,'bold')
canvas:attrColor("blue")
fuente,tamanio,estilo = canvas:attrFont()
texto = "Fuente: "..fuente..". Tamanio "..tamanio..". Estilo: "..estilo
canvas:drawText(10,60,texto)

canvas:attrFont("Tiresias",20,'bold')
canvas:attrColor("blue")
fuente,tamanio,estilo = canvas:attrFont()
texto = "Fuente: "..fuente..". Tamanio "..tamanio..". Estilo: "..estilo
canvas:drawText(10,100,texto)

canvas:attrFont("Tiresias",20,'bold-italic')
canvas:attrColor("blue")
fuente,tamanio,estilo = canvas:attrFont()
texto = "Fuente: "..fuente..". Tamanio "..tamanio..". Estilo: "..estilo
canvas:drawText(10,130,texto)

canvas:attrFont("Tiresias",10,'bold')
canvas:attrColor("blue")
fuente,tamanio,estilo = canvas:attrFont()
texto = "Fuente: "..fuente..". Tamanio "..tamanio..". Estilo: "..estilo
canvas:drawText(10,160,texto)



canvas:flush()