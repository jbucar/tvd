<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_right_24" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" right="30%">
				<region id="reg2" right="30%"/>
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1">
				<descriptorParam name="right" value="10%"/>
			</descriptor>
			<descriptor id="desc2" region="reg2">
				<descriptorParam name="right" value="10%"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="right" value="100%"/>
		</media>
	</body>
</ncl>
