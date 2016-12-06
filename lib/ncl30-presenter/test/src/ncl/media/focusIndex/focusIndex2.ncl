<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_focusIndex_2" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
			<region id="reg2"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg" focusIndex="2" selBorderColor="red" focusBorderWidth="2" />
			<descriptor id="desc2" region="reg2" focusIndex="1" focusBorderColor="green"/>
		</descriptorBase>
	</head>
	<body>
		<!-- falla: no asigna da importancia al elemento focusIndex -->
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="bounds" value="0%,0%,50%,50%"/>
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" >
			<property name="bounds" value="0%,50%,50%,50%"/>
			<property name="selBorderColor" value="white"/>
		</media>
	</body>
</ncl>
