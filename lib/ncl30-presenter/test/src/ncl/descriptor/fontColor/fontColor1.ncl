<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_fontColor_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="fontColor" value="white" />
				<descriptorParam name="fontSize" value="10" />
				<descriptorParam name="fontFamily" value="Tiresias" />
			</descriptor>
		</descriptorBase>
	</head>

	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../text/lorem.txt" descriptor="desc" />
	</body>
</ncl>
