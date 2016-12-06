<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="transitions" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<transitionBase>
			<transition id="tr_barWipeTop" type="barWipe" dur="2s" subtype="topToBottom" />
			<transition id="tr_barWipeLeft" type="barWipe" dur="2s" subtype="leftToRight" />
		</transitionBase>
		<regionBase>
			<region id="reg_bg" width="100%" height="100%" left="0%"  top="0%" />
			<region id="reg1" width="100%" height="50%" left="0%"  top="50%" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc_bg" region="reg_bg" />
			<descriptor id="desc1" region="reg1" transIn="tr_barWipeTop" transOut="tr_barWipeLeft" />
		</descriptorBase>
		<connectorBase>
			<causalConnector id="onBeginStart">
				<simpleCondition role="onBegin"/>
				<simpleAction role="start"/>
			</causalConnector>
			<causalConnector id="onEndStop">
				<simpleCondition role="onEnd"/>
				<simpleAction role="stop"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		
		<port id="pBg" component="bg" />
		<media id="bg" src="../resources/images/background.celeste.png" descriptor="desc_bg" >
			<area id="interval" begin="3s" end="8s" />
		</media>

		<media id="image" src="../resources/images/background.jpg" descriptor="desc1" />

		<link id="l1" xconnector="onBeginStart">
			<bind component="bg" interface="interval" role="onBegin"  />
			<bind component="image" role="start" />
		</link>
		<link id="l2" xconnector="onEndStop">
			<bind component="bg" interface="interval" role="onEnd"  />
			<bind component="image" role="stop" />
		</link> 
	</body>
</ncl>
