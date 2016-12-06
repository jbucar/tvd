<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_top_9" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="216px">
			    <region id="reg2"  />
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
			<descriptor id="desc2" region="reg2" >
				<descriptorParam name="top" value="72px" />
			</descriptor>
		</descriptorBase>
	</head>
	<body>
		<port id="port2" component="image2" />
		<media id="image2" src="../../../../images/blue.jpg" descriptor="desc2" />
	</body>
</ncl>
