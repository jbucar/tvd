<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_fontFamily_4" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="fontFamily" value="iTiresias, DejaVuSans" />
				<descriptorParam name="fontSize" value="20" />
			</descriptor>
		</descriptorBase>
	</head>

	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../text/lorem.txt" descriptor="desc" />
	</body>
</ncl>
