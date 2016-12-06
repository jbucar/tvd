<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_explicitDur_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="0%" left="0%" width="100%" height="100%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" explicitDur="1s"/>
		</descriptorBase>
	</head>
	<body>
		<port id="port1" component="lua1" />
		<media id="lua1" src="lua1.lua" descriptor="desc1"/>
	</body>
</ncl>
