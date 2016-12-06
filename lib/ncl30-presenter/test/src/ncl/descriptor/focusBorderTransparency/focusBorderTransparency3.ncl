<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_focusBorderTransparency_3" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="360" height="288"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" focusIndex="1" focusBorderTransparency="0" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
