<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_precedence_left_width_right_15" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" left="80%" width="10%" right="10%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="left" value="10%" />
				<descriptorParam name="width" value="10%" />
				<descriptorParam name="right" value="10%" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="left" value="72" />
			<property name="width" value="504" />
		</media>
	</body>
</ncl>
