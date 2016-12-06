<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_fontFamily_3" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
			</descriptor>
		</descriptorBase>
	</head>

	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../text/lorem.txt" descriptor="desc1" >
				<property name="fontFamily" value="Tiresias" />
				<property name="fontSize" value="10" />
		</media>
	</body>
</ncl>
