<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="id" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" width="50%" height="50%" left="10%" top="10%" zIndex="1" />
			<region id="reg2" width="50%" height="50%" left="20%" top="20%" zIndex="2" />
			<region id="reg3" width="50%" height="50%" left="30%" top="30%" zIndex="3" />
			<region id="reg4" width="50%" height="50%" left="40%" top="40%" zIndex="4" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
			<descriptor id="desc2" region="reg2" />
			<descriptor id="desc3" region="reg3" />
			<descriptor id="desc4" region="reg4" />
		</descriptorBase>
	</head>
	<body>
		<port id="entry4" component="media4" />
		<media id="media4" src="../resources/images/blue.jpg" descriptor="desc4" />

		<port id="entry3" component="media3" />
		<media id="media3" src="../resources/images/yellow.jpg" descriptor="desc3" />

		<port id="entry2" component="media2" />
		<media id="media2" src="../resources/images/green.jpg" descriptor="desc2" />

		<port id="entry1" component="media1" />
		<media id="media1" src="../resources/images/red.jpg" descriptor="desc1" />
	</body>
</ncl>
