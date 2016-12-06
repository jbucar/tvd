<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="buttonTestNCL" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
<head>
	<regionBase>
		<region id="base" width="100%" height="100%"/>
		<region id="button" width="10%" height="10%" left="45%" top="45%" />
	</regionBase>
	
	<descriptorBase>
		<descriptor id="baseDesc" region="base" focusIndex="0" />
		<descriptor id="buttonDesc" region="button" explicitDur="1s"/>
	</descriptorBase>
	<connectorBase>
		<causalConnector id="onKeySelectionStart">
		<connectorParam name="keyCode"/>
		<simpleCondition role="onSelection" key="$keyCode" />
		<simpleAction role="start" />
	</causalConnector>
</connectorBase>
</head>

<body>
	<port id="p0" component="fake"/>
	
	<media id="fake" src="../resources/images/transparent.png" descriptor="baseDesc"/>
	
	<media id="red" src="../resources/images/red.png" descriptor="buttonDesc"/>
	<media id="green" src="../resources/images/green.png" descriptor="buttonDesc"/>
	<media id="yellow" src="../resources/images/yellow.png" descriptor="buttonDesc"/>
	<media id="blue" src="../resources/images/blue.png" descriptor="buttonDesc"/>
	<media id="up" src="../resources/images/up.jpg" descriptor="buttonDesc"/>
	<media id="down" src="../resources/images/down.jpg" descriptor="buttonDesc"/>
	<media id="left" src="../resources/images/left.jpg" descriptor="buttonDesc"/>
	<media id="right" src="../resources/images/right.jpg" descriptor="buttonDesc"/>
	<media id="ok" src="../resources/images/ok.jpg" descriptor="buttonDesc"/>
	<media id="info" src="../resources/images/info.png" descriptor="buttonDesc"/>
	<media id="menu" src="../resources/images/menu.jpg" descriptor="buttonDesc"/>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="RED"/>
		</bind>
		<bind role="start" component="red" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="GREEN"/>
		</bind>
		<bind role="start" component="green" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="YELLOW"/>
		</bind>
		<bind role="start" component="yellow" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="BLUE"/>
		</bind>
		<bind role="start" component="blue" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="CURSOR_UP"/>
		</bind>
		<bind role="start" component="up" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="CURSOR_DOWN"/>
		</bind>
		<bind role="start" component="down" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="CURSOR_LEFT"/>
		</bind>
		<bind role="start" component="left" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="CURSOR_RIGHT"/>
		</bind>
		<bind role="start" component="right" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="OK"/>
		</bind>
		<bind role="start" component="ok" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="ENTER"/>
		</bind>
		<bind role="start" component="ok" />
	</link>
	
	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="INFO"/>
		</bind>
		<bind role="start" component="info" />
	</link>

	<link xconnector="onKeySelectionStart">
		<bind role="onSelection" component="fake" >
			<bindParam name="keyCode" value="MENU"/>
		</bind>
		<bind role="start" component="menu" />
	</link>	
</body>

</ncl>
