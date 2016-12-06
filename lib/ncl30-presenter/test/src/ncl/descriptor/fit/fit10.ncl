<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_fit_36" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg">
				<descriptorParam name="fit" value="hidden"/>
				<descriptorParam name="width" value="420"/>
				<descriptorParam name="height" value="114"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/lifia.png" descriptor="desc" />
	</body>
</ncl>
