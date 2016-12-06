<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_focusIndex_5" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
			<region id="reg2"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1"
				region="reg" 
				focusIndex="1" 
				focusBorderColor="white" 
				selBorderColor="green" 
				focusBorderWidth="2" 
				moveDown="2" />

			<descriptor id="desc2" 
				region="reg2" 
				focusIndex="2" 
				focusBorderColor="white" 
				focusBorderWidth="2"
				moveUp="1" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="bounds" value="25%,25%,50%,50%"/>
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" >
			<property name="bounds" value="0%,50%,50%,50%"/>
			<property name="visible" value="false"/>
		</media>
	</body>
</ncl>
