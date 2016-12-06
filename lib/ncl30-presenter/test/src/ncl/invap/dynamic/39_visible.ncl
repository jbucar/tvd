<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="visible1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="imgReg" left="0%" width="50%" />
			<region id="imgBG" top="100%" width="100%" />				
			<region id="txtReg" left="50%" width="50%"/>		
		</regionBase>
		<descriptorBase>
			<descriptor id="imgDesc" region="imgReg"/>
			<descriptor id="imgbg" region="imgBG"/>
			<descriptor id="txtDesc" region="txtReg"/>
		</descriptorBase>
		
		<connectorBase>
			<causalConnector id="onEndSet">
				<connectorParam name="newValue"/>
				<simpleCondition role="onEnd" />
				<simpleAction role="set" value="$newValue" max="unbounded"/>
			</causalConnector>
		</connectorBase>
	</head>
	
	<body>
		<port component="txt" id="entry1"/>
		<port component="img" id="entry2"/>
		<port component="bg" id="entry3"/>
		
		<media id="txt" descriptor="txtDesc" src="../resources/text/dynamic_visible_text.txt" >
			<property name="visible" value="true" />
		</media>
		<media id="img" descriptor="imgDesc" src="../resources/images/lifia.jpg" >
			<property name="visible" value="true" />
		</media>
		
		<media id="bg" descriptor="imgbg" src="../resources/images/transparent.png">
			<property name="bounds" value="0,0,720,576" />
			<area id="area1" begin="0s" end="4s"/>
			<area id="area2" begin="0s" end="8s"/>
			<area id="area3" begin="0s" end="12s"/>
			<area id="area4" begin="0s" end="16s"/>
			<area id="area5" begin="0s" end="20s"/>
			<area id="area6" begin="0s" end="24s"/>
		</media>
		
		<link xconnector="onEndSet">
			<bind role="onEnd" component="bg" interface="area1"/>
			<bind component="img" interface="visible" role="set">
				<bindParam name="newValue" value="false"/>
		 	</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="bg" interface="area2"/>
			<bind component="txt" interface="visible" role="set">
				<bindParam name="newValue" value="false"/>
		 	</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="bg" interface="area3"/>
			<bind component="img" interface="visible" role="set">
				<bindParam name="newValue" value="true"/>
		 	</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="bg" interface="area4"/>
			<bind component="txt" interface="visible" role="set">
				<bindParam name="newValue" value="true"/>
		 	</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="bg" interface="area5"/>
			<bind component="img" interface="visible" role="set">
				<bindParam name="newValue" value="false"/>
		 	</bind>
			<bind component="txt" interface="visible" role="set">
				<bindParam name="newValue" value="false"/>
		 	</bind>
		</link>
		<link xconnector="onEndSet">
			<bind role="onEnd" component="bg" interface="area6"/>
			<bind component="img" interface="visible" role="set">
				<bindParam name="newValue" value="true"/>
		 	</bind>
			<bind component="txt" interface="visible" role="set">
				<bindParam name="newValue" value="true"/>
		 	</bind>
		</link>
	</body>
</ncl>
