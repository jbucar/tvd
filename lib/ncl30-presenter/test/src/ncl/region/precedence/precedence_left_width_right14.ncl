<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_region_precedence_left_width_right_14" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" left="20%">
				<region id="reg2" right="10%" width="90%" />
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc2" />
	</body>
</ncl>
