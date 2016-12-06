<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_size_31" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="50%" height="50%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="size" value="invalidValue, 100%"/>
		</media>
	</body>
</ncl>
