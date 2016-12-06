<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_width_25" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" >
				<region id="reg2" />
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/red.jpg" descriptor="desc" >
			<property name="width" value="50%" />
		</media>
		<media id="image2" src="../../../../images/blue.jpg" descriptor="desc2" >
			<property name="width" value="50%" />
		</media>
	</body>
</ncl>
