<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_fontFamily_2" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
			</descriptor>
		</descriptorBase>
		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>
	</head>

	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../text/lorem.txt" descriptor="desc" >
				<property name="fontSize" value="10" />
				<property name="fontFamily" value="DejaVuSans" />
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image" role="onSelection"/>
			<bind component="image" role="set" interface="fontFamily" >
				<bindParam name="value" value="Tiresias"/>
			</bind>
		</link>
	</body>
</ncl>
