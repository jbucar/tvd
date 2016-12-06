<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="simpleAction08.08.01" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1"/>
			<region id="reg2"/>
			<region id="reg3"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1"/>
			<descriptor id="desc2" region="reg2"/>
			<descriptor id="desc3" region="reg3"/>
		</descriptorBase>
		<connectorBase>
			<causalConnector id="onSelectionStart">
				<simpleCondition key="RED" role="onSelection"/>
				<simpleAction max="unbounded" role="start"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		<port component="media1" id="psimpleAction08.08.01_1"/>
		<media id="media1" descriptor="desc1" src="../../../../images/blue.jpg">
			<property name="top" value="0"/>
			<property name="left" value="0%"/>
			<property name="width" value="50%"/>
			<property name="height" value="50%"/>
		</media>
		<media id="media2" descriptor="desc2" src="../../../../images/red.jpg">
			<property name="top" value="50%"/>
			<property name="left" value="0%"/>
			<property name="width" value="50%"/>
			<property name="height" value="50%"/>
		</media>
		<media id="media3" descriptor="desc3" src="../../../../images/yellow.jpg">
			<property name="height" value="50%"/>
			<property name="left" value="50%"/>
			<property name="width" value="50%"/>
			<property name="top" value="0%"/>
		</media>
		<link xconnector="onSelectionStart">
			<bind component="media1" role="onSelection"/>
			<bind component="media2" role="start"/>
			<bind component="media3" role="start"/> 
		</link>
	</body>
</ncl>
