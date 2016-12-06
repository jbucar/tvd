<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_media_event_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg1" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc1" region="reg1" >
			</descriptor>
		</descriptorBase>

		<connectorBase>
      		<causalConnector id="onKeySelectionStartAndStop">
        		<connectorParam name="keyCode"/>
        		<simpleCondition role="onSelection" key="$keyCode"/>
        		<compoundAction> 
          			<simpleAction role="start"/>
          			<simpleAction role="stop"/>
          		</compoundAction>
      		</causalConnector>
		</connectorBase>

	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >
			 
		</media>
	
	<link xconnector="onKeySelectionStartAndStop">
      <bind role="onSelection" component="image1"> 
      	<bindParam name="keyCode" value="RED"/>
      </bind>
      <bind role="stop" component="image1" />
      <bind role="start" component="image1" />
    </link>
	
	</body>
</ncl>
