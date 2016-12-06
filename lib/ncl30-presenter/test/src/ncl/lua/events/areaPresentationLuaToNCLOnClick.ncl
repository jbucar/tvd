<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="presentationNCLToLua" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">

<head>
	<regionBase>
		<region id="appReg"/>
		<region id="imageReg"/>
	</regionBase>

	<descriptorBase>
		<descriptor id="appDesc" region="appReg" focusIndex="1"/>
		<descriptor id="imageDesc" region="imageReg" />
	</descriptorBase>

	<connectorBase>
		<causalConnector id="onKeySelectionSet">
			<connectorParam name="keyCode"/>
			<simpleCondition role="onSelection" key="$keyCode"/> 
			<simpleAction role="start" />
		</causalConnector>
	</connectorBase>
</head>

<body>
   
	<port id="entryPoint" component="app"/>

	<media id="app" src="areaPresentationLuaToNCL.lua" descriptor="appDesc">
		<area id="a1" />
		<area id="a2" />
		<area id="a3" />
	</media>

	<media id="settings" type="application/x-ginga-settings" >
		<property name="service.currentKeyMaster" value="1"/>
	</media>

	<link xconnector="onKeySelectionSet">
		<bind role="onSelection" component="app"> 
			<bindParam name="keyCode" value="RED"/>
		</bind>
		<bind role="start" component="app" interface="a1"/>
	</link>

	<link xconnector="onKeySelectionSet">
		<bind role="onSelection" component="app"> 
			<bindParam name="keyCode" value="GREEN"/>
		</bind>
		<bind role="start" component="app" interface="a2"/>
	</link>

	<link xconnector="onKeySelectionSet">
		<bind role="onSelection" component="app"> 
			<bindParam name="keyCode" value="YELLOW"/>
		</bind>
		<bind role="start" component="app" interface="a3"/>
	</link>

</body>

</ncl>
