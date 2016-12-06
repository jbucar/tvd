<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="textAttributes" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" width="80%" height="40%" left="10%" top="10%" />
			<region id="reg2" width="80%" height="40%" left="10%" top="50%" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1">
				<descriptorParam name="fontFamily" value="Tiresias"/>
				<descriptorParam name="fontSize" value="22"/>
				<descriptorParam name="fontWeight" value="normal"/>
				<descriptorParam name="fontColor" value="white"/>
			</descriptor>
			<descriptor id="desc2" region="reg2">
				<descriptorParam name="fontFamily" value="Tiresias"/>
				<descriptorParam name="fontSize" value="22"/>
				<descriptorParam name="fontWeight" value="bold"/>
				<descriptorParam name="fontColor" value="blue"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="p1" component="text1" />
		<port id="p2" component="text2" />

		<media id="text1" src="../resources/text/textAttributes.txt" descriptor="desc1" />
		<media id="text2" src="../resources/text/textAttributes.txt" descriptor="desc2" />
	</body>
</ncl>
