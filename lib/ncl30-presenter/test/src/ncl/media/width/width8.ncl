<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_region_width_8" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="width" value="0px" />
		</media>
	</body>
</ncl>
