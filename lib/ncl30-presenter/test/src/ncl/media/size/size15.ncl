<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_size_15" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" height="50%" width="50%" >
			    <region id="reg2"/>
			 </region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" />
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>

	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc2" >
			<property name="size" value="50%,50%"/>
		</media>
	</body>
</ncl>
