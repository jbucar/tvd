<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_focusIndex_1a" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
			<region id="reg2"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg" focusIndex="1" focusBorderColor="red" selBorderColor="green" focusBorderWidth="2" />
			<descriptor id="desc2" region="reg2" focusBorderColor="green" focusBorderWidth="2"/>
		</descriptorBase>
	</head>
	<body>
		<!-- pasa: pasa pero no porque asignar el valor de focusIndex correctamente. -->
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="bounds" value="0%,0%,50%,50%"/>
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc1" >
			<property name="bounds" value="0%,50%,50%,50%"/>
			<property name="focusIndex" value="1"/>
			<property name="selBorderColor" value="white"/>
		</media>
	</body>
</ncl>
