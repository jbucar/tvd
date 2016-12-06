<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_soundLevel_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="video" />
		<media id="video" src="sbtvd-ts://0" descriptor="desc1" >-
			<property name="soundLevel" value="0,5" />
		</media>
	</body>
</ncl>

