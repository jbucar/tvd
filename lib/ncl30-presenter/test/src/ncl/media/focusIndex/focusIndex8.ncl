<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_focusIndex_8" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" 
				region="reg" 
				focusIndex="1" 
				focusBorderColor="white" 
				focusBorderWidth="2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<port id="port3" component="image3" />
		<port id="port4" component="image4" />

		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="bounds" value="0%,0%,50%,50%"/>
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc1" >
			<property name="bounds" value="0%,50%,50%,50%"/>
		</media>
		<media id="image3" src="../../../../images/green.jpg" descriptor="desc1" >
			<property name="bounds" value="50%,0%,50%,50%"/>
		</media>
		<media id="image4" src="../../../../images/yellow.jpg" descriptor="desc1" >
			<property name="bounds" value="50%,50%,50%,50%"/>
		</media>
	</body>
</ncl>
