<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_bounds_46" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="50%" left="50%" width="50%" height="50%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
			</descriptor>
		</descriptorBase>

	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
				<property name="bounds" value="50%,invalidValue,invalidValue,invalidValue" />
		</media>
	</body>
</ncl>

