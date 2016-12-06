<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_bottom_10" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
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
			<property name="bottom" value="576" />
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image" role="onSelection"/>
			<bind component="image" role="set" interface="bottom" >
				<bindParam name="value" value="-10"/>
			</bind>
		</link>
	</body>
</ncl>