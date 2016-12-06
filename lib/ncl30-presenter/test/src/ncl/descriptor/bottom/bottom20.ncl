<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_bottom_20" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg" >
				<descriptorParam name="bottom" value="37,5%" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc1" />
	</body>
</ncl>
