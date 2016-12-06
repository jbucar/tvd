<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<ruleBase>
			<rule comparator="eq" id="esCaption" value="es" var="system.caption"/>
			<rule comparator="ne" id="noEsCaption" value="es" var="system.caption"/>
		</ruleBase>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		
		<descriptorBase>
			<descriptorSwitch id="desc">
				<bindRule constituent="visible" rule="esCaption"/>
				<bindRule constituent="invisible" rule="noEsCaption"/>
				<descriptor id="visible" region="reg" />
				<descriptor id="invisible" region="reg" >
					<descriptorParam name="visible" value="false" />
				</descriptor>
			</descriptorSwitch>
		</descriptorBase>
		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>
	</head>
	<body>
		<port id="port" component="image" />

		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
