<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_fontColor_3" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
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
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../text/lorem.txt" descriptor="desc1" >
				<property name="width"/>
				<property name="height"/>
				<property name="fontColor" value="white" />
				<property name="fontSize" value="10" />
				<property name="fontFamily" value="Tiresias" />
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image1" role="onSelection"/>
			<bind component="image1" role="set" interface="fontColor" >
				<bindParam name="value" value="red"/>
			</bind>
			<bind component="image1" role="set" interface="width" >
				<bindParam name="value" value="50%"/>
			</bind>
			<bind component="image1" role="set" interface="height" >
				<bindParam name="value" value="50%"/>
			</bind>
		</link>
	</body>
</ncl>
