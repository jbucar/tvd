<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_bottom_8" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" >
			    <region id="reg2"  bottom="216px"/>
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="bottom" value="216px" />
			</descriptor>
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port2" component="image2" />
		<media id="image2" src="../../../../images/blue.jpg" descriptor="desc2" />
	</body>
</ncl>
