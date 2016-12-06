<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="device_first" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
        <head>
                <regionBase id="1" device="systemAudio(0)">
                        <region height="50%" id="bgReg" left="0%" top="0%" width="100%" zIndex="0"/>
                </regionBase>

		<regionBase id="2" device="systemScreen(0)">
                        <region height="50%" id="bgReg2" left="0%" top="50%" width="100%" zIndex="0"/>
                </regionBase>

                <descriptorBase>
                        <descriptor id="blueDesc" region="bgReg"/>
			<descriptor id="greenDesc" region="bgReg2"/>
                </descriptorBase>
        </head>
        <body id="body">
                <port id="p1" component="image"/>
		<port id="p2" component="green"/>

                <media descriptor="blueDesc" id="image" src="../../../../images/blue.jpg"/>
		<media descriptor="greenDesc" id="green" src="../../../../images/green.jpg"/>
        </body>
</ncl>
