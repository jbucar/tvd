<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_region_width_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<descriptorBase>
			<descriptor id="desc" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/red.jpg">
			<property name="bounds" value="0%,50%,50%,50%" />
		</media>
		<media id="image2" src="../../../../images/blue.jpg" >
			<property name="bounds" value="0%,0%,50%,50%" />
		</media>
	</body>
</ncl>
