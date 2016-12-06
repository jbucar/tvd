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
			<causalConnector id="onKeySelectionSetStop">
				<connectorParam name="keyCode"/> 
				<connectorParam name="var"/>
				<simpleCondition role="onSelection" key="$keyCode"/> 
				<compoundAction>
					<simpleAction role="set" value="$var"  max="unbounded" qualifier="seq"/>
					<simpleAction role="stop" max="unbounded" />
				</compoundAction>
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		<port id="portLua" component="lua" />
		<port id="portImage" component="image" />

		<media id="settings" type="application/x-ginga-settings" >
			<property name="default.focusBorderColor" value="blue"/>
		</media>

		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="bounds" value="0%,0%,100%,100%"/>
		</media>

		<media id="lua" src="updateFocusBorderColor.lua" descriptor="desc" >
			<property name="attr" />
		</media>

		<link xconnector="onKeySelectionSetStop">
			<bind role="onSelection" component="lua"> 
				<bindParam name="keyCode" value="RED"/>
			</bind>
			<bind component="settings" role="set" interface="default.focusBorderColor" >
				<bindParam name="var" value="green"/>
			</bind>
			<bind role="set" component="lua" interface="attr">
				<bindParam name="var" value="red"/>
			</bind>
			<bind role="stop" component="image" />
		</link>

	</body>
</ncl>
