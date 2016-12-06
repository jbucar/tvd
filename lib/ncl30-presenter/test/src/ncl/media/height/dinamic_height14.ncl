<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_height_14" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="height" value="100%" />
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image" role="onSelection"/>
			<bind component="image" role="set" interface="height" >
				<bindParam name="value" value="288"/>
			</bind>
		</link>
	</body>
</ncl>
