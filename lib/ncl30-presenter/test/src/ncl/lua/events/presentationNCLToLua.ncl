<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="presentationNCLToLua" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">

<head>
	<regionBase>
		<region id="appReg"/>
		<region id="otroReg" />
	</regionBase>

	<descriptorBase>
		<descriptor id="appDesc" region="appReg" zIndex="1"/>
		<descriptor id="otroDesc" region="otroReg" focusIndex="1" zIndex="0"/>
	</descriptorBase>

	<connectorBase>
		<causalConnector id="onKeySelectionSet">
			<connectorParam name="keyCode"/> 
			<connectorParam name="var"/>
			<simpleCondition role="onSelection" key="$keyCode"/> 
			<simpleAction role="set" value="$var"/>
		</causalConnector>
	</connectorBase>
</head>

<body>
   
	<port id="entryPoint" component="app"/>
	<port id="entryPoint2" component="otro"/>

	<media id="otro" src="../../../../transparent.png" descriptor="otroDesc"/>

	<media id="app" src="presentationNCLToLua.lua" descriptor="appDesc"/>

	<media id="settings" type="application/x-ginga-settings" >
		<property name="service.currentKeyMaster" value="1"/>
	</media>

</body>

</ncl>