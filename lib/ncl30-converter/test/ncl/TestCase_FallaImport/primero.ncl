<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="primero" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region height="100%" id="bgReg" left="0%" top="0%" width="100%" zIndex="0"/>
			<region height="95" id="redReg" left="50" top="50" width="89" zIndex="2"/>
			<region height="95" id="greenReg" left="150" top="50" width="89" zIndex="2"
			<region height="95" id="yellowReg" left="50" top="160" width="89" zIndex="2"/>
			<region height="95" id="blueReg" left="150" top="160" width="89" zIndex="2"/>
			<region height="205" id="detailReg" left="0" top="375" width="100%" zIndex="2"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="detailDescriptor" region="detailReg"/>
			<descriptor id="redDesc" region="redReg"/>
			<descriptor id="greenDesc" region="greenReg"/>
			<descriptor id="yellowDesc" region="yellowReg"/>
			<descriptor id="blueDesc" region="blueReg"/>
		</descriptorBase>
	</head>
	<body id="body">		
		<port id="p1" component="greenButton"/>

		<media descriptor="greenDesc" id="greenButton" src="resources/images/green.jpg"/>
			<property name="src"/>
		</media>
	</body>
</ncl>
