<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_transparency_10" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="sharedReg" />
		</regionBase>
		<descriptorBase>
			<descriptor id="descRed" region="sharedReg" >
				<descriptorParam name="transparency" value="70%" />
			</descriptor>
			<descriptor id="descBlue" region="sharedReg" >
				<descriptorParam name="transparency" value="50%" />
			</descriptor>
		</descriptorBase>

	</head>
	<body>
		<port id="portRed" component="imageRed" />
		<port id="portBlue" component="imageBlue" />
		<media id="imageRed" src="../../../../images/red.jpg" descriptor="descRed" />
		<media id="imageBlue" src="../../../../images/blue.jpg" descriptor="descBlue" />
	</body>
</ncl>
