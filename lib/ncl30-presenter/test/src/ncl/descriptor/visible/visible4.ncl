<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptorParams_visible_3b" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="visible" value="" />
			</descriptor>
		</descriptorBase>
	</head>

	<!-- Se va a probar la propiedad visible en descriptorParams-->
	<!-- descriptorParams name="visible" value="" -->

	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" />
	</body>
</ncl>
