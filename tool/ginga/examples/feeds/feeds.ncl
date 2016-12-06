<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="feedsPublica" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="appReg" />
		</regionBase>

		<descriptorBase>
			<descriptor id="appDesc" region="appReg" focusIndex="0"/>
		</descriptorBase>
	</head>
	<body id="body">
		<port id="appPort" component="app"/>

		<media id="app" descriptor="appDesc" src="app.lua" >
			<property name="feeds"/>
    </media>

		<!-- Settings -->
		<media id="settings" type="application/x-ginga-settings" >
			<property name="service.currentKeyMaster" value="0"/>
		</media>
	</body>
</ncl>		