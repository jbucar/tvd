<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_fit_5" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" width="50%" height="50%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1"/>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/paisaje.jpg" descriptor="desc1" >
			<property name="fit" value="fill" />
		</media>
	</body>
</ncl>
