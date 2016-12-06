<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_zIndex_12" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" width="360" height="288">
				<region id="reg2" width="100" height="100" />
				<region id="reg3" top="50" left="50" width="100" height="100"/>
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="zIndex" value="3"/>
			</descriptor>
			<descriptor id="desc2" region="reg2" >
				<descriptorParam name="zIndex" value="2"/>
			</descriptor>
			<descriptor id="desc3" region="reg3">
				<descriptorParam name="zIndex" value="1"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<port id="port3" component="image3" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" />
		<media id="image2" src="../../../../images/green.jpg" descriptor="desc2" />
		<media id="image3" src="../../../../images/red.jpg" descriptor="desc3" />
	</body>
</ncl>
