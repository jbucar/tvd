<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_fontFamily_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
			</descriptor>
		</descriptorBase>
		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>
	</head>

	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../text/lorem.txt" descriptor="desc1" >
				<property name="fontSize" value="10" />
				<property name="fontFamily" value="Tiresias" />
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image" role="onSelection"/>
			<bind component="image" role="set" interface="fontFamily" >
				<bindParam name="value" value="DejaVuSans"/>
			</bind>
		</link>
	</body>
</ncl>
