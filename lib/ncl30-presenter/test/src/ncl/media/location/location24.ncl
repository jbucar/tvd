<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_location_24" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" top="20%" left="20%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="location" value="50%,50%"/>
		</media>
	</body>
</ncl>
