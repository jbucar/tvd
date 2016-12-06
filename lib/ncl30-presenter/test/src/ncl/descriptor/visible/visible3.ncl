<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptorParams_visible_3a" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="visible"/>
			</descriptor>
		</descriptorBase>
	</head>

	<!-- Se va a probar la propiedad visible en descriptorParams-->
	<!-- descriptorParams name="visible" -->

	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
