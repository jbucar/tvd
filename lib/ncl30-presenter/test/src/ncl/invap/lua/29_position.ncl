<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="position" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="bgRegion" width="100%" height="100%" left="0%" top="0%" zIndex="1" />
			<region id="reg" width="100%" height="100%" left="0%" top="0%" zIndex="2" />
		</regionBase>
		<descriptorBase>
			<descriptor id="bgDescriptor" region="bgRegion"/>
			<descriptor id="desc" region="reg"/>
		</descriptorBase>
	</head>
	<body>
		<port id="pBg" component="bg" />
		<media id="bg" src="../resources/images/background.celeste.png" descriptor="bgDescriptor" />

		<port id="pLua" component="lua" />
		<media id="lua" src="29_position.lua" descriptor="desc" >
			<property name="top" value="10%" />
			<property name="left" value="10%" />
			<property name="width" value="80%" />
			<property name="height" value="80%" />
		</media>
	</body>
</ncl>
