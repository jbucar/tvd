<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="buttonTestLUA" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
<head>
	<regionBase>
		<region id="base" width="100%" height="100%"/>
	</regionBase>
	<descriptorBase>
		<descriptor id="baseDesc" region="base" focusIndex="appFocus"/>
	</descriptorBase>

</head>

<body>
	<port id="p1" component="script"/>
	
	<media id="varGlobal" type="application/x-ginga-settings" descriptor="baseDesc">
		<property name="service.currentKeyMaster" value="appFocus"/>
	</media>
	
	<media id="script" src="41_lua.lua" descriptor="baseDesc"/>

</body>

</ncl>
