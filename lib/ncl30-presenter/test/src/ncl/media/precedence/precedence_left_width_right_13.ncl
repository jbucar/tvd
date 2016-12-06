<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_precedence_left_width_right_13" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" left="72" width="576" right="72"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="left" value="15%" />
				<descriptorParam name="width" value="70%" />
				<descriptorParam name="right" value="15%" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="left" value="72" />
			<property name="right" value="72" />
		</media>
	</body>
</ncl>
