<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_area_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="0%" left="0%" width="100%" height="100%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1"/>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" interface="area1" />
		<media id="image1" src="../../../../images/green.jpg" descriptor="desc1">
			<area id="area1" label="labelArea1"/>
		</media>
	</body>
</ncl>
