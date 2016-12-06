<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_fit_3" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1"/>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/paisaje.jpg" descriptor="desc1">
				<property name="fit" value="fill"/>
				<property name="width" value="720"/>
				<property name="height" value="576"/>
		</media>
	</body>
</ncl>
