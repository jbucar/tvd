<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_2" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
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

		<media id="settings" type="application/x-ginga-settings" >
			<property name="shared.appHeight" value="100"/>
		</media>

		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="height"/>
		</media>

		<link xconnector="con#onRedSelectionSet">
			<bind component="image" role="onSelection"/>

			<bind component="settings" role="set" interface="shared.appHeight" >
				<bindParam name="value" value="288"/>
			</bind>

			<bind role="getValue" component="settings" interface="shared.appHeight" />
			<bind component="image" role="set" interface="height" >
				<bindParam name="value" value="$getValue"/>
			</bind>
		</link>

	</body>
</ncl>
