<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="nclClicks" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">

<head>
	<regionBase>
		<region id="appReg"/>
		<region id="imageReg"/>
	</regionBase>

	<descriptorBase>
		<descriptor id="appDesc" region="appReg"  focusIndex="1"/>
		<descriptor id="imageDesc" region="imageReg" />
	</descriptorBase>

	<connectorBase>
		<causalConnector id="onEndAttributionStopSetStart">
			<connectorParam name="var"/>
			<simpleCondition role="onEndAttribution"/>
			<simpleAction role="set" value="$var"/>
		</causalConnector>
	</connectorBase>
</head>

<body>
   
	<port id="entryPoint" component="app"/>
	<port id="entryPoint2" component="image"/>

	<media id="app" src="attributionLuaToNCL.lua" descriptor="appDesc">
		<property name="newBounds"/>
	</media>

	<media id="image" src="../../../../images/blue.jpg" descriptor="imageDesc">
		<property name="bounds" value="0,0,360,288"/>
	</media>

	<media id="settings" type="application/x-ginga-settings" >
		<property name="service.currentKeyMaster" value="1"/>
	</media>
	
	<link xconnector="onEndAttributionStopSetStart">
		<bind role="onEndAttribution" component="app" interface="newBounds"/>
		<bind role="getValue" component="app" interface="newBounds"/>
		<linkParam name="var" value="$getValue"/>
		<bind role="set" component="image" interface="bounds"/>
	</link>

</body>

</ncl>
