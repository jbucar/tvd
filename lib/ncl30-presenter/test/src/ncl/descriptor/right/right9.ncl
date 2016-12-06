<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_right_9" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"  right="288px">
			    <region id="reg2" />
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1"/>		
			<descriptor id="desc2" region="reg2">
			      <descriptorParam name="right" value="108px"/>
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc2" />
	</body>
</ncl>

