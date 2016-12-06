<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_focusIndex_4" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regBlue"/>
			<region id="regRed"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="descBlue" 
				region="regBlue" 
				moveUp="4" 
				moveDown="2" 
				moveLeft="4" 
				moveRight="3" 
				focusIndex="1" 
				focusBorderColor="white"
				focusBorderWidth="-2">
			</descriptor>

			<descriptor id="descRed"
				region="regRed"
				moveUp="1" 
				moveDown="3" 
				moveLeft="3" 
				moveRight="4" 
				focusIndex="2" 
				focusBorderColor="white"
				focusBorderWidth="-2">
			</descriptor>

		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="imageBlue" />
		<port id="port2" component="imageRed" />

		<media id="imageBlue" src="../../../../images/blue.jpg" descriptor="descBlue" >
			<property name="bounds" value="0%,0%,50%,50%"/>
		</media>
		<media id="imageRed" src="../../../../images/red.jpg" descriptor="descRed" >
			<property name="bounds" value="0%,50%,50%,50%"/>
		</media>

	</body>
</ncl>
