<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_width_22" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="100%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="width" value="50%" />
		</media>
	</body>
</ncl>