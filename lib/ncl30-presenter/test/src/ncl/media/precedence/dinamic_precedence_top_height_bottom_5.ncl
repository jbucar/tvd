<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_precedence_top_height_bottom_5" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
		</descriptorBase>
		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="top" value="30%" />
			<property name="height" value="30%" />
			<property name="bottom" value="40%" />
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image1" role="onSelection"/>
			<bind component="image1" role="set" interface="top" >
				<bindParam name="value" value="173"/>
			</bind>
			<bind component="image1" role="set" interface="height" >
				<bindParam name="value" value="173"/>
			</bind>
			<bind component="image1" role="set" interface="bottom" >
				<bindParam name="value" value="172"/>
			</bind>
		</link>
	</body>
</ncl>
