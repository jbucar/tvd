<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_zIndex_18" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg0" width="100" height="100" zIndex="4" />
			<region id="reg1" width="100" height="100" zIndex="2">
				<region id="reg2"  top="50" left="50" width="100" height="100"/>
				<region id="reg3"  top="75" left="75" width="100" height="100"/>
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg0" />
			<descriptor id="desc2" region="reg2" />
			<descriptor id="desc3" region="reg3" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<port id="port3" component="image3" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" >
		</media>
		<media id="image3" src="../../../../images/green.jpg" descriptor="desc3" >
			<property name="zIndex" value="3" />
		</media>
	</body>
</ncl>
