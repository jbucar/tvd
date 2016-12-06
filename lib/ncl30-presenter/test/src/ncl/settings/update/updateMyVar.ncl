<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
		<connectorBase>
			<causalConnector id="onKeySelectionSet">
				<connectorParam name="keyCode"/> 
				<connectorParam name="var"/>
				<simpleCondition role="onSelection" key="$keyCode"/> 
				<simpleAction role="set" value="$var"  max="unbounded" qualifier="seq"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		<port id="portLua" component="lua" />
		<port id="portImage" component="image" />

		<media id="settings" type="application/x-ginga-settings" >
			<property name="shared.myVar" value="value"/>
		</media>

		<media id="lua" src="updateMyVar.lua" descriptor="desc" >
			<property name="attr" />
		</media>

		<link xconnector="onKeySelectionSet">
			<bind role="onSelection" component="lua"> 
				<bindParam name="keyCode" value="RED"/>
			</bind>
			<bind component="settings" role="set" interface="shared.myVar" >
				<bindParam name="var" value="val"/>
			</bind>
			<bind role="set" component="lua" interface="attr">
				<bindParam name="var" value="red"/>
			</bind>
		</link>

	</body>
</ncl>
