<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="cocineros" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="appReg"/>
			<region height="37" id="infoReg" left="670" top="526" width="37" zIndex="2" />
			<region height="100%" id="bgReg" left="0%" top="0%" width="100%" zIndex="0"/>
			<region height="95" id="redReg" left="50" top="50" width="89" zIndex="2"/>
			<region height="95" id="greenReg" left="150" top="50" width="89" zIndex="2"/>
			<region height="95" id="yellowReg" left="50" top="160" width="89" zIndex="2"/>
			<region height="95" id="blueReg" left="150" top="160" width="89" zIndex="2"/>
			<region height="200" id="detailReg" left="0" top="375" width="100%" zIndex="2"/>
			<region height="100%" id="videoReg" left="0%" top="0%" width="100%" zIndex="1"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="appDesc" region="appReg"/>
			<descriptor id="bgDesc" region="bgReg"/>
			<descriptor id="infoDesc" region="infoReg" />
			<descriptor id="detailDescriptor" region="detailReg"/>
			<descriptor id="redDesc" region="redReg"/>
			<descriptor id="greenDesc" region="greenReg"/>
			<descriptor id="yellowDesc" region="yellowReg"/>
			<descriptor id="blueDesc" region="blueReg"/>
			<descriptor id="videoDesc" region="videoReg"/>
		</descriptorBase>
		<connectorBase>
			<causalConnector id="onKeySelectionStopSetStart">
				<connectorParam name="keyCode"/>
				<connectorParam name="var"/>
				<simpleCondition key="$keyCode" role="onSelection"/>
				<compoundAction operator="seq">
					<simpleAction max="unbounded" qualifier="seq" role="stop"/>
					<simpleAction max="unbounded" qualifier="seq" role="set" value="$var"/>
					<simpleAction max="unbounded" qualifier="seq" role="start"/>
				</compoundAction>
			</causalConnector>
			<causalConnector id="onKeySelectionStopStart">
				<connectorParam name="keyCode"/>
				<simpleCondition key="$keyCode" role="onSelection"/>
				<compoundAction operator="seq">
					<simpleAction max="unbounded" qualifier="seq" role="stop"/>
					<simpleAction max="unbounded" qualifier="seq" role="start"/>
				</compoundAction>
			</causalConnector>
			<causalConnector id="onKeySelectionSet">
				<connectorParam name="keyCode"/>
				<connectorParam name="var"/>
				<simpleCondition key="$keyCode" role="onSelection"/>
				<simpleAction role="set" value="$var"/>				
			</causalConnector>
		</connectorBase>
	</head>	
	<body>
		<port id="entry" component="buttonToInfo"/>
		<port id="entr2y" component="video"/>

		<media descriptor="videoDesc" id="video" src="sbtvd-ts://0">
			<property name="bounds"/>
		</media>

		<media id="application" src="main.lua" descriptor="appDesc">
			<property name="pressedKey" />
		</media>
		<media id="buttonToMenu" src="resources/images/ok.png" descriptor="infoDesc"/>
		<media id="buttonToInfo" src="resources/images/ok.png" descriptor="infoDesc"/>

		<media id="background" src="resources/images/background.png" descriptor="bgDesc"/>

		<media descriptor="redDesc" id="redButton" src="resources/images/red.jpg"/>
		<media descriptor="greenDesc" id="greenButton" src="resources/images/green.jpg"/>
		<media descriptor="yellowDesc" id="yellowButton" src="resources/images/yellow.jpg"/>
		<media descriptor="blueDesc" id="blueButton" src="resources/images/blue.jpg"/>
		<media descriptor="detailDescriptor" id="detail" src="resources/images/initial_detail.png"/>

		<media descriptor="detailDescriptor" id="blueDetail" src="resources/images/blue_detail.png"/>
		<media descriptor="detailDescriptor" id="greenDetail" src="resources/images/green_detail.png"/>
		<media descriptor="detailDescriptor" id="yellowDetail" src="resources/images/yellow_detail.png"/>
		<media descriptor="detailDescriptor" id="redDetail" src="resources/images/red_detail.png"/>

		<link xconnector="onKeySelectionSet">
			<bind component="application" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_LEFT"/>
			</bind>
			<bind component="application" role="set" interface="pressedKey">
				<bindParam name="var" value="left" />
			</bind>
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="application" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_RIGHT"/>
			</bind>
			<bind component="application" role="set" interface="pressedKey" >
				<bindParam name="var" value="right" />
			</bind>
		</link>

		<link xconnector="onKeySelectionSet">
			<bind component="application" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_DOWN"/>
			</bind>
			<bind component="application" role="set" interface="pressedKey" >
				<bindParam name="var" value="down" />
			</bind>
		</link>


		<link xconnector="onKeySelectionSet">
			<bind component="application" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_UP"/>
			</bind>
			<bind component="application" role="set" interface="pressedKey" >
				<bindParam name="var" value="up" />
			</bind>
		</link>

		<link xconnector="onKeySelectionStopStart">
			<bind component="greenButton" role="onSelection">
				<bindParam name="keyCode" value="RED"/>
			</bind>
			<bind component="detail" role="stop"/>
			<bind component="blueDetail" role="stop"/>
			<bind component="greenDetail" role="stop"/>
			<bind component="yellowDetail" role="stop"/>
			<bind component="redDetail" role="start"/>
		</link>

		<link xconnector="onKeySelectionStopStart">
			<bind component="greenButton" role="onSelection">
				<bindParam name="keyCode" value="BLUE"/>
			</bind>
			<bind component="detail" role="stop"/>
			<bind component="redDetail" role="stop"/>
			<bind component="greenDetail" role="stop"/>
			<bind component="yellowDetail" role="stop"/>
			<bind component="blueDetail" role="start"/>
		</link>

		<link xconnector="onKeySelectionStopStart">
			<bind component="greenButton" role="onSelection">
				<bindParam name="keyCode" value="GREEN"/>
			</bind>
			<bind component="detail" role="stop"/>
			<bind component="blueDetail" role="stop"/>
			<bind component="redDetail" role="stop"/>
			<bind component="yellowDetail" role="stop"/>
			<bind component="greenDetail" role="start"/>
		</link>

		<link xconnector="onKeySelectionStopStart">
			<bind component="greenButton" role="onSelection">
				<bindParam name="keyCode" value="YELLOW"/>
			</bind>
			<bind component="detail" role="stop"/>
			<bind component="blueDetail" role="stop"/>
			<bind component="greenDetail" role="stop"/>
			<bind component="redDetail" role="stop"/>
			<bind component="yellowDetail" role="start"/>
		</link>

		<link xconnector="onKeySelectionStopSetStart">
			<bind component="greenButton" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_UP"/>
			</bind>
			<bind component="buttonToInfo" role="stop"/>
			<bind component="detail" role="stop"/>
			<bind component="blueDetail" role="stop"/>
			<bind component="greenDetail" role="stop"/>
			<bind component="redDetail" role="stop"/>
			<bind component="yellowDetail" role="stop"/>
			<bind component="blueButton" role="stop"/>
			<bind component="greenButton" role="stop"/>
			<bind component="redButton" role="stop"/>
			<bind component="yellowButton" role="stop"/>
			<bind component="video" interface="bounds" role="set">
				<bindParam name="var" value="38.8%, 5%, 54.8%, 55.2%"/>
			</bind>
			<bind component="background" role="start"/>
			<bind component="application" role="start"/>
		</link>

		<!--interactivity OFF-->
		<link xconnector="onKeySelectionStopSetStart">
			<bind component="application" role="onSelection">
				<bindParam name="keyCode" value="OK"/>
			</bind>
			<bind component="background" role="stop"/>
			<bind component="application" role="stop"/>
			<bind component="video" interface="bounds" role="set">
				<bindParam name="var" value="0%, 0%, 100%, 100%"/>
			</bind>
			<bind component="buttonToMenu" role="start"/>
		</link>

		<!--interactivity ON-->
		<link xconnector="onKeySelectionStopSetStart">
			<bind component="buttonToMenu" role="onSelection">
				<bindParam name="keyCode" value="OK"/>
			</bind>
			<bind component="buttonToMenu" role="stop"/>
			<bind component="video" interface="bounds" role="set">
				<bindParam name="var" value="38.8%, 5%, 54.8%, 55.2%"/>
			</bind>
			<bind component="background" role="start"/>
			<bind component="application" role="start"/>
		</link>

		<!--interactivity ON-->
		<link xconnector="onKeySelectionStopSetStart">
			<bind component="buttonToInfo" role="onSelection">
				<bindParam name="keyCode" value="OK"/>
			</bind>
			<bind component="buttonToInfo" role="stop"/>
			<bind component="video" interface="bounds" role="set">
				<bindParam name="var" value="38.8%, 5%, 54.8%, 55.2%"/>
			</bind>
			<bind component="background" role="start"/>
			<bind component="redButton" role="start"/>
			<bind component="yellowButton" role="start"/>
			<bind component="greenButton" role="start"/>
			<bind component="blueButton" role="start"/>
			<bind component="detail" role="start"/>
		</link>

		<!--interactivity OFF-->
		<link xconnector="onKeySelectionStopSetStart">
			<bind component="application" role="onSelection">
				<bindParam name="keyCode" value="OK"/>
			</bind>
			<bind component="background" role="stop"/>
			<bind component="application" role="stop"/>
			<bind component="video" interface="bounds" role="set">
				<bindParam name="var" value="0%, 0%, 100%, 100%"/>
			</bind>
			<bind component="buttonToMenu" role="start"/>
		</link>

		<!--interactivity OFF-->
		<link xconnector="onKeySelectionStopSetStart">
			<bind component="greenButton" role="onSelection">
				<bindParam name="keyCode" value="OK"/>
			</bind>
			<bind component="background" role="stop"/>
			<bind component="application" role="stop"/>
			<bind component="detail" role="stop"/>
			<bind component="blueDetail" role="stop"/>
			<bind component="greenDetail" role="stop"/>
			<bind component="redDetail" role="stop"/>
			<bind component="yellowDetail" role="stop"/>
			<bind component="blueButton" role="stop"/>
			<bind component="greenButton" role="stop"/>
			<bind component="redButton" role="stop"/>
			<bind component="yellowButton" role="stop"/>
			<bind component="video" interface="bounds" role="set">
				<bindParam name="var" value="0%, 0%, 100%, 100%"/>
			</bind>
			<bind component="buttonToInfo" role="start"/>
		</link>

		<link xconnector="onKeySelectionStopSetStart">
			<bind component="greenButton" role="onSelection">
				<bindParam name="keyCode" value="CURSOR_UP"/>
			</bind>
			<bind component="background" role="stop"/>
			<bind component="application" role="stop"/>
			<bind component="detail" role="stop"/>
			<bind component="blueDetail" role="stop"/>
			<bind component="greenDetail" role="stop"/>
			<bind component="redDetail" role="stop"/>
			<bind component="yellowDetail" role="stop"/>
			<bind component="blueButton" role="stop"/>
			<bind component="greenButton" role="stop"/>
			<bind component="redButton" role="stop"/>
			<bind component="yellowButton" role="stop"/>
			<bind component="video" interface="bounds" role="set">
				<bindParam name="var" value="38.8%, 5%, 54.8%, 55.2%"/>
			</bind>
			<bind component="background" role="start"/>
			<bind component="application" role="start"/>
		</link>

	</body>




</ncl>
