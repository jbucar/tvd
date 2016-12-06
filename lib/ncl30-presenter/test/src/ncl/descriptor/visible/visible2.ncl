<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptorParams_visible_2" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg">
				<descriptorParam name="visible" value="false" />
			</descriptor>
		</descriptorBase>
	</head>

	<!-- Se va a probar la propiedad visible en descriptorParams-->
	<!-- descriptorParams name="visible" value="false" -->

	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
