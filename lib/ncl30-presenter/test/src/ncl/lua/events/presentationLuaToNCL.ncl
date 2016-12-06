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
		<causalConnector id="onEndStart">
			<simpleCondition role="onEnd"/> 
			<simpleAction role="start"/>
		</causalConnector>
	</connectorBase>
</head>

<body>
   
	<port id="entryPoint" component="app"/>

	<media id="image" src="../../../../images/blue.jpg" descriptor="imageDesc"/>

	<media id="app" src="presentationLuaToNCL.lua" descriptor="appDesc"/>

	<media id="settings" type="application/x-ginga-settings" >
		<property name="service.currentKeyMaster" value="1"/>
	</media>

	<link xconnector="onEndStart">
		<bind role="onEnd" component="app"/>
		<bind role="start" component="image"/>
	</link>

</body>

</ncl>
