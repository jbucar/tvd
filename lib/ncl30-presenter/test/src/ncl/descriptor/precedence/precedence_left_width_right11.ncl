<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor _precedence_left_width_right_11" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" left="72px" width="576px" right="73px"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="left" value="360px" />
				<descriptorParam name="width" value="360px" />
				<descriptorParam name="right" value="100px" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" />
	</body>
</ncl>
