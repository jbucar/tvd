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
		<port id="port" component="image" />

		<media id="settings" type="application/x-ginga-settings" >
			<property name="system.language" value="READ_ONLY"/>
		</media>

		<media id="image" src="../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
