<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_moveLeft-moveRight-moveUp-moveDown_3" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regBlue"/>
			<region id="regRed"/>
			<region id="regGreen"/>
			<region id="regYellow"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="descBlue" 
				region="regBlue" 
				moveUp="4" 
				moveDown="2" 
				moveLeft="4" 
				moveRight="3" 
				focusIndex="1" 
				focusBorderColor="white" />

			<descriptor id="descRed"
				region="regRed"
				moveUp="1" 
				moveDown="3" 
				moveLeft="3" 
				moveRight="4" 
				focusIndex="2" 
				focusBorderColor="white" />

			<descriptor id="descGreen" 
				region="regGreen"
 				moveUp="2" 
				moveDown="4" 
				moveLeft="1" 
				moveRight="2" 
				focusIndex="3" 
				focusBorderColor="white" />

			<descriptor id="descYellow"
				region="regYellow"
 				moveUp="3" 
				moveDown="1" 
				moveLeft="2" 
				moveRight="1" 
				focusIndex="4" 
				focusBorderColor="white"/>

		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="imageBlue" />
		<port id="port2" component="imageRed" />
		<port id="port3" component="imageGreen" />
		<port id="port4" component="imageYellow" />

		<media id="imageBlue" src="../../../../images/blue.jpg" descriptor="descBlue" >
			<property name="bounds" value="0%,0%,50%,50%"/>
		</media>
		<media id="imageRed" src="../../../../images/red.jpg" descriptor="descRed" >
			<property name="bounds" value="0%,50%,50%,50%"/>
		</media>
		<media id="imageGreen" src="../../../../images/green.jpg" descriptor="descGreen" >
			<property name="bounds" value="50%,0%,50%,50%"/>
		</media>
		<media id="imageYellow" src="../../../../images/yellow.jpg" descriptor="descYellow" >
			<property name="bounds" value="50%,50%,50%,50%"/>
		</media>
	</body>
</ncl>
