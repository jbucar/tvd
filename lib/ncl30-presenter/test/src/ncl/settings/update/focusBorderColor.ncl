<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="lua" />

		<media id="settings" type="application/x-ginga-settings" >
			<property name="default.focusBorderColor" value="red"/>
		</media>

		<media id="lua" src="focusBorderColor.lua"/>
	</body>
</ncl>
