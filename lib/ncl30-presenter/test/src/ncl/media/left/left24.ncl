<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_left_24" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" left="30%">
				<region id="reg2" left="30%"/>
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1">
				<descriptorParam name="left" value="10%"/>
			</descriptor>
			<descriptor id="desc2" region="reg2">
				<descriptorParam name="left" value="10%"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="left" value="100%"/>
		</media>
	</body>
</ncl>
