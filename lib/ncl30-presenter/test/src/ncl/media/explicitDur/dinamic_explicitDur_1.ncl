<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_dinamic_explicitDur_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" top="0%" left="0%" width="100%" height="100%"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1"/>
		</descriptorBase>
		<connectorBase>
			<causalConnector id="onEndStart">
				<simpleCondition role="onEnd"/>
				<simpleAction role="start"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/green.jpg" descriptor="desc1">
			<property name="explicitDur" value="1s"/>
		</media>
		<media id="image2" src="../../../../images/blue.jpg" descriptor="desc1"/>
		<link xconnector="onEndStart">
			<bind component="image1" role="onEnd"/>
			<bind component="image2" role="start"/>
		</link>
	</body>
</ncl>
