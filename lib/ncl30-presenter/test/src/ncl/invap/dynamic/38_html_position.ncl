<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="testApp" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
				<region id="leftReg"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="leftDesc" region="leftReg"/>					
		</descriptorBase>
		
		<connectorBase>
			<causalConnector id="onEndSet">
				<connectorParam name="newValue"/>
				<simpleCondition role="onEnd"/>
				<compoundAction operator="seq">
					<simpleAction role="stop" min="0" max="unbounded" qualifier="seq"/>
					<simpleAction role="start" min="0" qualifier="seq"/>
					<simpleAction role="set" value="$newValue" min="0" max="unbounded" qualifier="seq"/>	
				</compoundAction>
			</causalConnector>
			
			<causalConnector id="onBeginStart">
				<simpleCondition role="onBegin"/>
				<simpleAction role="start"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>					
		<port component="background" id="e0"/>
			
		<media  id="html" descriptor="leftDesc" src="../resources/html/webpage.html">
			<property name="left"/>
			<property name="right"/>
			<property name="top" />
			<property name="bottom"/> 
			<property name="width"/>
			<property name="height"/>
			<property name="bounds"/>
			<property name="size"/>
			<property name="location"/>
		</media>
		<!-- background -->
		<media id="background" descriptor="leftDesc" src="../resources/images/background.celeste.png">
			<area id="a1" begin="0s" end="2s"/>
			<area id="a2" begin="0s" end="4s"/>
			<area id="a3" begin="0s" end="6s"/>
			<area id="a4" begin="0s" end="8s"/>
			<area id="a5" begin="0s" end="10s"/>
			<area id="a6" begin="0s" end="12s"/>
			<area id="a7" begin="0s" end="14s"/>
			<area id="a8" begin="0s" end="16s"/>
			<area id="a9" begin="0s" end="18s"/>
		</media>
		
		<link xconnector="onBeginStart">
			<bind role="onBegin" component="background"/>
			<bind role="start" component="html"/>
		</link>
		
		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a1"/>
			<bind role="set" component="html" interface="right">
				<bindParam name="newValue" value="10%"/>
			</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a2"/>
			<bind role="set" component="html" interface="width">
				<bindParam name="newValue" value="40%"/>
			</bind>
		</link>
		
		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a3"/>
			<bind role="set" component="html" interface="left">
				<bindParam name="newValue" value="60%"/>
			</bind>
		</link>
		
		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a4"/>
			<bind role="set" component="html" interface="top">
				<bindParam name="newValue" value="20%"/>
			</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a5"/>
			<bind role="set" component="html" interface="bottom">
				<bindParam name="newValue" value="60%"/>
			</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a6"/>
			<bind role="set" component="html" interface="height">
				<bindParam name="newValue" value="30%"/>
			</bind>
		</link>

		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a7"/>
			<bind role="set" component="html" interface="location">
				<bindParam name="newValue" value="10%,10%"/>
			</bind>
		</link>

		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a8"/>
			<bind role="set" component="html" interface="size">
				<bindParam name="newValue" value="80%,80%"/>
			</bind>
		</link>

		<link xconnector="onEndSet">
			<bind role="onEnd" component="background" interface="a9"/>
			<bind role="set" component="html" interface="bounds">
				<bindParam name="newValue" value="25%,25%,50%,50%"/>
			</bind>
		</link>

	</body>
</ncl>
