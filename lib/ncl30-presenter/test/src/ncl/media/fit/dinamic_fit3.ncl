<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_fit_3" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
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
			<property name="width" />
			<property name="height" />
			<property name="fit" value="fill"/>
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image1" role="onSelection"/>
			<bind component="image1" role="set" interface="fit" >
				<bindParam name="value" value="slice"/>
			</bind>
			<bind component="image1" role="set" interface="width" >
				<bindParam name="value" value="50%"/>
			</bind>
			<bind component="image1" role="set" interface="height" >
				<bindParam name="value" value="50%"/>
			</bind>
		</link>
	</body>
</ncl>
