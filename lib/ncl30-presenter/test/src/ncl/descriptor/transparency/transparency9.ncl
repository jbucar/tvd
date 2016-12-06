<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_transparency_9" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regRed" />
			<region id="regBlue" />
		</regionBase>
		<descriptorBase>
			<descriptor id="descRed" region="regRed" >
				<descriptorParam name="transparency" value="70%" />
			</descriptor>
			<descriptor id="descBlue" region="regBlue" >
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
