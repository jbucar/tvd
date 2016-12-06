<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_settings_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" />
		</descriptorBase>
		<connectorBase>

			<causalConnector id="onRedSelectionStopSetStar">
				<connectorParam name="value"/>
				<simpleCondition key="RED" role="onSelection"/>
				<compoundAction>	
					<simpleAction role="stop" max="unbounded" qualifier="seq"/>
					<simpleAction role="set" value="$value" max="unbounded" qualifier="seq"/>
					<simpleAction role="start" max="unbounded" qualifier="seq"/>
				</compoundAction>	
			</causalConnector>
			
		</connectorBase>

	</head>
	<body>
		<port id="port1" component="image1" />
		<port id="port2" component="image2" />

		<media id="settings" type="application/x-ginga-settings" >
			<property name="default.selBorderColor" />
		</media>

		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc" >
			<property name="bounds" value="0%,0%,50%,50%"/>
			<property name="focusBorderWidth" value="-2"/>
			<property name="focusIndex" value="1"/>
		</media>

		<media id="image2" src="../../../../images/red.jpg" descriptor="desc" >
			<property name="bounds" value="0%,50%,50%,50%"/>
		</media>


		<link xconnector="onRedSelectionStopSetStar">
			<bind component="image2" role="onSelection"/>
			<bind component="image1" role="stop"/>
			<bind component="settings" role="set" interface="default.selBorderColor" >
				<bindParam name="value" value="red"/>
			</bind>
			<bind component="image1" role="start"/>
		</link>

	</body>
</ncl>

