<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_precedence_left_width_right_10" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="80%" height="10%" bottom="10%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="top" value="10%" />
				<descriptorParam name="height" value="10%" />
				<descriptorParam name="bottom" value="80%" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="top" value="37.5%" />
			<property name="height" value="37.5%" />
			<property name="bottom" value="25%" />
		</media>
	</body>
</ncl>
