<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_fit_30a" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1">
				<descriptorParam name="width" value="50%"/>
				<descriptorParam name="height" value="50%"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/paisaje.jpg" descriptor="desc1" >
				<property name="fit" value="slice"/>
		</media>
	</body>
</ncl>
