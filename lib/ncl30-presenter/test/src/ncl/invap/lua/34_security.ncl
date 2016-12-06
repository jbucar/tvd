<?xml version="1.0" encoding="ISO-8859-1"?>
 <ncl id="LuaTest" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>			
			<region height="100%" id="appRegion" left="0%" top="0%" width="100%"/>
		</regionBase>
		<descriptorBase>			
			<descriptor focusIndex="appFocus" id="appDescriptor" region="appRegion"/>
		</descriptorBase>	
	</head>
	<body>
		<port component="application" id="start"/>
		<media id="foco" type="application/x-ginga-settings">
			<property name="service.currentKeyMaster" value="appFocus"/>
		</media>
		<media descriptor="appDescriptor" id="application" src="34_security.lua" type="application/x-ginga-NCLua"/>
	</body>
</ncl>
