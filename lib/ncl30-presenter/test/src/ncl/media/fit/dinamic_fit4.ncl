<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_fit_4" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1"/>
		</descriptorBase>
		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/paisaje.jpg" descriptor="desc1">
			<property name="location"/>
			<property name="fit" value="meet"/>
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image1" role="onSelection"/>
			<bind component="image1" role="set" interface="fit" >
				<bindParam name="value" value="meetBest"/>
			</bind>
		</link>
	</body>
</ncl>
