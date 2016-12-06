<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="vresize" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region height="100%" id="videoReg" left="0%" top="0%" width="100%" zIndex="1"/>
			<region height="6.51%" id="intReg" left="90.5%" top="90%" width="5.07%" zIndex="2"/>
		</regionBase>
		<descriptorBase>			
			<descriptor id="videoDesc" region="videoReg"/>			
			<descriptor id="intDesc" region="intReg"/>
		</descriptorBase>
		<connectorBase>
			<causalConnector id="onKeySelectionSet">
				<connectorParam name="keyCode"/>
				<connectorParam name="var"/>
				<simpleCondition key="$keyCode" role="onSelection"/>
				<compoundAction operator="seq">
					<simpleAction max="unbounded" role="set" value="$var"/>
				</compoundAction>
			</causalConnector>
			<causalConnector id="onKeySelectionStop">
				<connectorParam name="keyCode"/>
				<connectorParam name="var"/>
				<simpleCondition key="$keyCode" role="onSelection"/>
				<simpleAction max="unbounded" role="stop"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>		

		<port component="video" id="entryVideo"/>
		
		
		<media id="foco" type="application/x-ginga-settings">
			<property name="service.currentKeyMaster" value="1"/>
		</media>
		

		<media  id="video" descriptor="videoDesc" src="sbtvd-ts://0">
			<property name="bounds"/>
			<property name="width" value="360"/>
			<property name="height" value="288"/>
		</media>
		
		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="RED"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="0%, 0%, 100%, 100%"/>
			</bind>			
			
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="GREEN"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="60%, 0%, 40%, 60%"/>
			</bind>			

		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="YELLOW"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="60%, 60%, 40%, 40%"/>
			</bind>			
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="BLUE"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="34.5%, 21.5%, 54.21%, 20.89%"/>
			</bind>			
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="ENTER"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="20%, 20%,  60%, 60%"/>
			</bind>			
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_UP"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="0%, 0%, 100%, 20%"/>
			</bind>			
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_DOWN"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="0%, 80%, 100%, 20%"/>
			</bind>			
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_LEFT"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="0%, 0%, 20%, 100%"/>
			</bind>			
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="video" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_RIGHT"/>
			</bind>
			<bind component="video" interface="bounds" role="set">
					<bindParam name="var" value="80%, 0%, 20%, 100%"/>
			</bind>			
		</link>


	</body>
</ncl>
