<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_focusBorderTransparency_2" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="360" height="288"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg"/>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="focusBorderTransparency" value="0.5"/>
			<property name="focusIndex" value="1"/>
		</media>
	</body>
</ncl>
