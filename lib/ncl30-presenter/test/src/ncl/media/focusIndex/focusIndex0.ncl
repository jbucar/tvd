<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_focusIndex_0" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg" focusIndex="2" focusBorderColor="red" selBorderColor="green" focusBorderWidth="2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="bounds" value="25%,25%,50%,50%"/>
		</media>
	</body>
</ncl>
