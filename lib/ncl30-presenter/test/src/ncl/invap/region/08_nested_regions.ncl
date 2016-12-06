<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="testTop" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
			<regionBase>
				<region id="img1Reg" top="10%" left="10%" height="80%" width="80%" >
					<region id="img2Reg" top="10%" left="10%" height="80%" width="80%" />
				</region>
			</regionBase>
			<descriptorBase>
				<descriptor id="img1Desc" region="img1Reg"/>
				<descriptor id="img2Desc" region="img2Reg"/>
			</descriptorBase>
	</head>

	<body>
		<port id="p0" component="img1"/>
		<port id="p1" component="img2"/>
		<media id="img1" src="../resources/images/image4.jpg" descriptor="img1Desc" />
		<media id="img2" src="../resources/images/nested.png" descriptor="img2Desc" />
	</body>
</ncl>
