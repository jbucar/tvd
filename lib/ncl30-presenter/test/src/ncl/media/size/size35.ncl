<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_size_35" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="360px" height="288px"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="size" value="invalidValue,invalidValue"/>
		</media>
	</body>
</ncl>