<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_zIndex_13" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"  width="100" height="100"/>
			<region id="reg2"  top="50" left="50" width="100" height="100" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="zIndex" value="3" />
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" >
			<property name="zIndex" value="2" />
		</media>
	</body>
</ncl>
