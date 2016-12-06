<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<ruleBase>
			<rule comparator="eq" id="subtitle" value="en" var="system.subtitle"/>
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

		<switch id="switch">
			<bindRule constituent="image" rule="subtitle"/>
			<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
		</switch>
	</body>
</ncl>
