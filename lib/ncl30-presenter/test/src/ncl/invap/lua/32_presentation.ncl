<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="main" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">

	<head>
		<regionBase>
			<region id="regImagen1" width="100%" height="100%" zIndex="2"/>
			<region id="scriptLua" width="100%" height="100%" left="0%" top="0%" zIndex="2" />
		</regionBase>
		<descriptorBase>
			<descriptor id="descImagen1"  region="regImagen1" />
			<descriptor id="descLua" region="scriptLua" focusIndex="appFocus"/>
		</descriptorBase>
		<connectorBase>
			<causalConnector id="onBeginStart">
				<simpleCondition role="onBegin"/>
				<simpleAction role="start" />				
			</causalConnector>
			<causalConnector id="onEndStop">
				<simpleCondition role="onEnd"/>
				<simpleAction role="stop" />				
			</causalConnector>
		</connectorBase>
	</head>

	<body>
		<port id="pEntrada" component="LuaPresentation"/>
		<media id="foco" type="application/x-ginga-settings">
			<property name="service.currentKeyMaster" value="appFocus"/>
		</media>
		
		<port id="pImagen" component="imagen1" />
		<media id="imagen1" src="../resources/images/lifia.jpg" descriptor="descImagen1"/> 
		<media id="LuaPresentation" src="32_presentation.lua" descriptor="descLua" >
				<area id="areaDeImagen" label="area"/>
		</media>
			
		<link xconnector="onBeginStart">
			<bind role="onBegin" component="LuaPresentation" interface="areaDeImagen" />
			<bind role="start" component="imagen1"/>
		</link>
	
		<link xconnector="onEndStop">
			<bind role="onEnd" component="LuaPresentation" interface="areaDeImagen" />
			<bind role="stop" component="imagen1" />
		</link>
	</body>
</ncl>
