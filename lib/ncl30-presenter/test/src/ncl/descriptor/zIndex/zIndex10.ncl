<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_zIndex_10" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" zIndex="2" width="360" height="288">
				<region id="reg2" width="50" height="50" />
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="zIndex" value="3"/>
			</descriptor>
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" />
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" />
	</body>
</ncl>
