<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_location_19" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" left="50%">
			    <region id="reg2" left="50%"/>
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


		<media id="image2" src="../../../../images/blue.jpg" descriptor="desc2" >
			<property name="left" value="10%" />
		</media>
	</body>
</ncl>
