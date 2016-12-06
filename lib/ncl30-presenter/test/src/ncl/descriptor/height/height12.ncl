<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_height_16" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1">
			    <region id="reg2"  height="50%"/>
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
				<descriptorParam name="height" value="37.5%" />
			</descriptor>
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>

	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc2" />
	</body>
</ncl>
