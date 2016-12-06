<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_bounds_5" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
			</descriptor>
		</descriptorBase>
		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>

	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			 <property name="top" value=""/>
		         <property name="bounds" value="0%,25%,50%,50%" />
		</media>
		<link xconnector="con#onRedSelectionSet">
			<bind component="image1" role="onSelection"/>
			<bind component="image1" role="set" interface="top" >
				<bindParam name="value" value="50%"/>
			</bind>
		</link>
	</body>	
</ncl>

