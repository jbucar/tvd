<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_zIndex_7" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" width="100" height="100"/>
			<region id="reg2" top="50" left="50" width="200" height="200" />
			<region id="reg3" top="200" left="200" width="100" height="100"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="zIndex" value="1"/>
			</descriptor>
			<descriptor id="desc2" region="reg2" >
				<descriptorParam name="zIndex" value="2"/>
			</descriptor>
			<descriptor id="desc3" region="reg3">
				<descriptorParam name="zIndex" value="3"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="text1" />
		<port id="port3" component="image2" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" />
		<media id="text1" src="../../../../text/lorem.txt" descriptor="desc2" >
			<property name="fontSize" value="10" />		
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc3" />
	</body>
</ncl>
