<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<ruleBase>
			<rule comparator="eq" id="focusBorderTransparency" value="37.5%" var="default.focusBorderTransparency"/>
		</ruleBase>
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
		<port id="port" component="switch" />

		<media id="settings" type="application/x-ginga-settings" >
			<property name="default.focusBorderTransparency" value="37.5%"/>
		</media>

		<switch id="switch">
			<bindRule constituent="image" rule="focusBorderTransparency"/>
			<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
				<property name="bounds" value="0%,0%,50%,50%"/>
				<property name="focusBorderWidth" value="-3"/>
				<property name="focusIndex" value="1"/>
			</media>
		</switch>
	</body>
</ncl>
