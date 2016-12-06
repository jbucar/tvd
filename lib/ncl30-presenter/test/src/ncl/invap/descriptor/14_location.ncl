<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="testWidthLeft" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
			<regionBase>
				<region id="imgReg" height="30%" top="0%" width="30%" left="0%"/>
			</regionBase>
			<descriptorBase>
				<descriptor id="img1Desc" region="imgReg">
					<descriptorParam name="location" value="70%, 70%"/>					
				</descriptor>
				<descriptor id="img2Desc" region="imgReg"/>
			</descriptorBase>
	</head>

	<body>
		<port id="p0" component="img1"/>
		<port id="p1" component="img2"/>
		<media id="img1" src="../resources/images/image7.jpg" descriptor="img1Desc" />
		<media id="img2" src="../resources/images/image7.jpg" descriptor="img2Desc" />
	</body>
</ncl>
