<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_focusSrc_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" 
				region="reg" 
				focusIndex="1" 
				focusSrc="../../../../images/blue.jpg"
				focusBorderWidth="2"/>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/red.jpg" descriptor="desc" >
			<property name="bounds" value="25%,25%,50%,50%" />
		</media>
	</body>
</ncl>
