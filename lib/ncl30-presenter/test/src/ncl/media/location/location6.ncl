<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_location_6" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="location" value="37.5%,37.5%"/>
		</media>
	</body>
</ncl>
