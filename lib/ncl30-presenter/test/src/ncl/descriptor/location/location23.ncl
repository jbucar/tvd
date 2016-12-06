<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_location_19" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" >
			    <region id="reg2"/>
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="location" value="50%,50%" />
			</descriptor>
			<descriptor id="desc2" region="reg2" >
				<descriptorParam name="location" value="101%,101%" />
			</descriptor>
		</descriptorBase>

	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/red.jpg" descriptor="desc1" />
		<media id="image2" src="../../../../images/blue.jpg" descriptor="desc2" />
	</body>
</ncl>
