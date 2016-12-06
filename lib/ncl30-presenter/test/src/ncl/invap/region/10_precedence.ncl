<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="testTop" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
			<regionBase>
				<region id="imgReg" right="40%" left="10%" width="80%" bottom="50%" top="10%" height="80%"/>
			</regionBase>
			<descriptorBase>
				<descriptor id="imgDesc" region="imgReg"/>
			</descriptorBase>
	</head>

	<body>
		<port id="p0" component="img"/>
		<media id="img" src="../resources/images/image3.jpg" descriptor="imgDesc" />
	</body>
</ncl>
