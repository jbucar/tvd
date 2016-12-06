<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<ruleBase>
			<rule comparator="eq" id="selBorderColor" value="red" var="default.selBorderColor"/>
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
			<property name="default.selBorderColor" value="red"/>
		</media>

		<switch id="switch">
			<bindRule constituent="image" rule="selBorderColor"/>
			<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
				<property name="bounds" value="25%,25%,50%,50%"/>
				<property name="focusBorderWidth" value="2"/>
				<property name="focusIndex" value="1"/>
			</media>
		</switch>
	</body>
</ncl>
