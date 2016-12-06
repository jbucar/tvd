<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="main" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regImagen1" width="100%" height="100%" zIndex="1"/>
			<region id="regImagen2" width="100%" height="100%" zIndex="2"/>	
		</regionBase>
		
		<descriptorBase>
			<descriptor id="descImagen1"  region="regImagen1" explicitDur="20s"  />
			<descriptor id="descImagen2"  region="regImagen2"  focusIndex="appFocus"/>
		</descriptorBase>

		<connectorBase>
			<causalConnector id="onEndAttributionSet">
				<simpleCondition role="onEndAttribution"/>
				<connectorParam name="var" />
				<simpleAction role="set" value="$var"/>
			</causalConnector>
		</connectorBase>
			
	</head>
	<body>
		<port id="pEntrada" component="LuaCambioSrc"/>
		<port id="pEntrada2" component="imagen2"/>

		<media id="imagen2" src="../resources/images/image8.jpg" descriptor="descImagen2">
			<property name="src"/>
		</media>
		<media id="foco" type="application/x-ginga-settings">
			<property name="service.currentKeyMaster" value="appFocus"/>
		</media>
	
		<media id="LuaCambioSrc" src="33_attribution.lua" descriptor="descImagen2">
			<property name="srcNueva"/>
		</media>			
			
		<link xconnector="onEndAttributionSet">
			<bind component="LuaCambioSrc" interface="srcNueva" role="onEndAttribution"/>
			<bind component="LuaCambioSrc" interface="srcNueva" role="getValue"/>
			<linkParam name="var" value="$getValue"/>
			<bind component="imagen2" interface="src" role="set"/>
		</link>	
	</body>
	
</ncl>
