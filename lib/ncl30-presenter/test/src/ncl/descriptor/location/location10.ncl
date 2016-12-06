<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_location_10" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
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
				<descriptorParam name="location" value="50%,50%" />
			</descriptor>
		</descriptorBase>

	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc2" />
	</body>
</ncl>
