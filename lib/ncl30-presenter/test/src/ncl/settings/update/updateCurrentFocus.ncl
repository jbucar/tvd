<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="descLua" region="reg" focusIndex="1"/>
			<descriptor id="descImage" region="reg" focusIndex="2"/>
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
			<property name="service.currentFocus" value="0"/>
			<property name="default.focusBorderWidth" value="0"/>
		</media>

		<media id="image" src="../../../../images/blue.jpg" descriptor="descImage" >
			<property name="bounds" value="0%,0%,100%,100%"/>
		</media>

		<media id="lua" src="updateCurrentFocus.lua" descriptor="descLua" >
			<property name="attr" />
		</media>

		<link xconnector="onKeySelectionSetStop">
			<bind role="onSelection" component="image"> 
				<bindParam name="keyCode" value="RED"/>
			</bind>
			<bind component="settings" role="set" interface="service.currentFocus" >
				<bindParam name="var" value="1"/>
			</bind>
			<bind role="set" component="lua" interface="attr">
				<bindParam name="var" value="val"/>
			</bind>
			<bind role="stop" component="image" />
		</link>

	</body>
</ncl>
