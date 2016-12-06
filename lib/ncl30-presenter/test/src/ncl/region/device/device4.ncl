<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="device_first" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
        <head>
                <regionBase id="1" device="systemAudio(0)">
                        <region height="100%" id="bgReg" left="0%" top="0%" width="100%" zIndex="0"/>
                </regionBase>

                <descriptorBase>
                        <descriptor id="blueDesc" region="bgReg"/>
                </descriptorBase>
        </head>
        <body id="body">
                <port id="p1" component="image"/>

                <media descriptor="blueDesc" id="image" src="../../../../images/blue.jpg"/>
        </body>
</ncl>
