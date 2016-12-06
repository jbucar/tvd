<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_precedence_left_width_right_5" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="left" value="10%" />
			<property name="width" value="80%" />
			<property name="right" value="10%" />
		</media>
	</body>
</ncl>
