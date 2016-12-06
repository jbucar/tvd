<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor _precedence_left_width_right_14" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" left="72px" width="360px" right="73px"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="left" value="360px" />
				<descriptorParam name="right" value="160px" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
