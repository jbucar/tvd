<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_precedence_left_width_right_11" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="72" height="576" bottom="72"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="top" value="15%" />
				<descriptorParam name="height" value="62.5%" />
				<descriptorParam name="bottom" value="15%" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="top" value="12.5%" />
			<property name="bottom" value="12.5%" />
		</media>
	</body>
</ncl>
