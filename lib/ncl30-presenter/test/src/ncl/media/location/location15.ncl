<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_location_15" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="37.5%" left="37.5%" >
			    <region id="reg2" />
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>

	</head>
	<body>
		<port id="port" component="image" />
		<port id="port2" component="image2" />
		<media id="image" src="../../../../images/red.jpg" descriptor="desc1" />
		<media id="image2" src="../../../../images/blue.jpg" descriptor="desc2" >
			<property name="location" value="50%, 50%" /> 
		</media>
	</body>
</ncl>
