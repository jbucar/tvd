<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_fontWeight_3" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="fontWeight" value="bolde" />
				<descriptorParam name="fontFamily" value="Tiresias" />
				<descriptorParam name="fontSize" value="10" />
			</descriptor>
		</descriptorBase>
	</head>

	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../text/lorem.txt" descriptor="desc" />
	</body>
</ncl>