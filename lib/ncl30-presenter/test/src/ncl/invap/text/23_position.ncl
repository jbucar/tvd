<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="text_position" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="100%" height="100%" left="0%" top="0%" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg"/>
		</descriptorBase>
	</head>
	<body>
		<port id="pBg" component="bg" />
		<media id="bg" src="../resources/images/background.celeste.png" descriptor="desc" />
		<port id="pText" component="text" />
		<media id="text" src="../resources/text/text_long.txt" descriptor="desc">
			<property name="fontSize" value="22" />
			<property name="left" value="10%" />
			<property name="top" value="10%" />
			<property name="width" value="90%" />
			<property name="height" value="80%" />
		</media>
	</body>
</ncl>
