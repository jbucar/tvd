<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_location_44" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" left="50%">
			    <region id="reg2"/>
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>

	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" />

		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" >
			<property name="location" value="-10%, 0" />
		</media>
	</body>
</ncl>

