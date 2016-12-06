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

	<media id="app" src="areaPresentationLuaToNCL.lua" descriptor="appDesc">
		<area id="a1" begin="1s" end="2s"/>
		<area id="a2" begin="3s" end="4s"/>
		<area id="a3" begin="2s" end="5s"/>
	</media>

	<media id="settings" type="application/x-ginga-settings" >
		<property name="service.currentKeyMaster" value="1"/>
	</media>

</body>

</ncl>
