<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_focusIndex_7a" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
			<region id="reg2"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1"
				region="reg" 
				focusIndex="1" 
				focusBorderColor="white" 
				selBorderColor="green" 
				focusBorderWidth="2"
				moveDown="2"/>
			<descriptor id="desc2" 
				region="reg2" 
				focusIndex="2" 
				focusBorderColor="white" 
				focusBorderWidth="2"
				moveUp="1"/>
		</descriptorBase>

		<connectorBase>
			<importBase documentURI="../../conectores.ncl" alias="con"/>
		</connectorBase>
	</head>
	<body>
		<port id="port3" component="settings" />
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />

		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			<property name="bounds" value="0%,0%,50%,50%"/>
		</media>
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" >
			<property name="bounds" value="0%,50%,50%,50%"/>
		</media>

		<media id="settings" type="application/x-ginga-settings" >
			<property name="service.currentFocus" value="2"/>
			<property name="service.currentKeyMaster" value="image2"/>
		</media>

		<link xconnector="con#onRedSelectionSet">
			<bind component="image1" role="onSelection"/>
			<bind component="settings" role="set" interface="service.currentKeyMaster" >
				<bindParam name="value" value="image2"/>
			</bind>
			<bind component="settings" role="set" interface="service.currentFocus" >
				<bindParam name="value" value="2"/>
			</bind>
		</link>

		<link xconnector="con#onRedSelectionSet">
			<bind component="image2" role="onSelection"/>
			<bind component="settings" role="set" interface="service.currentKeyMaster" >
				<bindParam name="value" value="image1"/>
			</bind>
			<bind component="settings" role="set" interface="service.currentFocus" >
				<bindParam name="value" value="1"/>
			</bind>
		</link>
	</body>
</ncl>
