<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="segundo" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region height="205" id="recetaReg" left="0" top="275" width="100%" zIndex="2"/>
			<region height="375" id="ingredientesReg" left="0" top="0" width="260" zIndex="2"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="recetaDesc" region="recetaReg"/>
			<descriptor id="ingredientesDesc" region="ingredientesReg"/>
		</descriptorBase>
	</head>
	<body id="body">

		<port id="p1" component="receta"/>

		<media descriptor="recetaDesc" id="receta" src="resources/images/Recetas_Preparacion1.png">
			<property name="src"/>
		</media>
	</body>
</ncl>



