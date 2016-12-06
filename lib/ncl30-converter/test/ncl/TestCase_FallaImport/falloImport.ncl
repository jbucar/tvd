<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="testCase" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		
		<importedDocumentBase>
			<importNCL alias="ncl1" documentURI="primero.ncl"/>
			<importNCL alias="ncl2" documentURI="segundo.ncl"/>
		</importedDocumentBase>
		
		<regionBase>
			<region height="100%" id="bgReg" left="0%" top="0%" width="100%" zIndex="0"/>
		</regionBase>
		
		<descriptorBase>
			<descriptor id="backgroundDesc" region="bgReg"/>
		</descriptorBase>
		
	</head>
	<body>
		<port component="video" id="background"/>
		
		<media descriptor="backgroundDesc" id="background" src="resources/images/background.png"/>
	</body>
</ncl>
