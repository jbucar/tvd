<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor _precedence_top_height_bottom_15" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" bottom="50px" top="72" height="50px"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="height" value="432" />
				<descriptorParam name="bottom" value="100px" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
