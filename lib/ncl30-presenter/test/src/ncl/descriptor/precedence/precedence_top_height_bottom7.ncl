<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor _precedence_top_height_bottom_7" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="top" value="12.5%" />
				<descriptorParam name="height" value="75%" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
