<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_precedence_top_height_bottom_10" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" bottom="30%" top="30%" height="30%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="top" value="50%" />
				<descriptorParam name="height" value="60%" />
				<descriptorParam name="bottom" value="20%" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
