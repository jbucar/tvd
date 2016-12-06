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
      		<causalConnector id="onKeySelectionStopAndStart">
        		<connectorParam name="keyCode"/>
        		<simpleCondition role="onSelection" key="$keyCode"/> 
          		<compoundAction> 
          			<simpleAction role="stop"/>
          			<simpleAction role="start"/>
          		</compoundAction> 
      		</causalConnector>
      		
      		<causalConnector id="onStart">
        		<simpleCondition role="onStart"/> 
          		<compoundAction> 
          			<simpleAction role="stop"  max="unbounded" qualifier="seq"/>
          			<simpleAction role="start" max="unbounded" qualifier="seq"/>
          		</compoundAction> 
      		</causalConnector>
		</connectorBase>

	</head>
	<body>
		<port id="port1" component="image1" />
		<media id="image1" src="../../../../images/blue.jpg" descriptor="desc1" >			 
		</media>
		
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc1" >			 
		</media>
		
		<media id="image3" src="../../../../images/green.jpg" descriptor="desc1" >			 
		</media>
	
		<link xconnector="onKeySelectionStopAndStart">
      		<bind role="onSelection" component="image1"> 
      			<bindParam name="keyCode" value="RED"/>
      		</bind>
      		<bind role="stop" component="image1" />
			<bind role="start" component="image2" />
    	</link>
		<link xconnector="onStart">
			<bind role="onStart" component="image2" />
			<bind role="stop" component="image2" />
			<bind role="start" component="image3" />
		</link>
		
		<link xconnector="onStart">
			<bind role="onStart" component="image3" />
			<bind role="stop" component="image3" />
			<bind role="start" component="image1" />
		</link>
	</body>
</ncl>
